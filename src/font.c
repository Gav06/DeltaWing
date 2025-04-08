#include <stdio.h>
#include <string.h>

#include "renderer.h"
#include "font.h"


typedef struct Block {
    uint8_t type;
    uint32_t size;
} Block_t;

// Some helper functions to turn bytes into ints and strings
// Note that the parse_ functions only query the buffers, they do 
// not write to or modify them in any way. The read_ functions do that.

uint32_t parse_uint32(const uint8_t *buf, size_t bufSize, size_t index) {
    if (index + 4 > bufSize) {
        fprintf(stderr, "Error: Attempted uint32_t read, byte index out of range.\n");
        return 0;
    }

    return (uint32_t) (
        buf[index] |
        buf[index + 1] << 8 |
        buf[index + 2] << 16 |
        buf[index + 3] << 24
    );
}

int32_t parse_int32(const uint8_t *buf, size_t bufSize, size_t index) {
    if (index + 4 > (bufSize /  sizeof(uint8_t))) {
        fprintf(stderr, "Error: Attempted int32_t read, byte index out of range.\n");
        return 0;
    }

    return (int32_t) (
        buf[index] |
        buf[index + 1] << 8 |
        buf[index + 2] << 16 |
        buf[index + 3] << 24
    );
}

uint16_t parse_uint16(const uint8_t *buf, size_t bufSize, size_t index) {
    if (index + 2 > (bufSize /  sizeof(uint8_t))) {
        fprintf(stderr, "Error: Attempted uint16_t read, byte index out of range.\n");
        return 0;
    }

    return (uint16_t) (
        buf[index] |
        buf[index + 1] << 8
    );
}

// Reads byte-by-byte until a null terminator is reached, thanks stackoverflow
char* parse_string(const uint8_t *buf, size_t bufSize, size_t strSize) {
    if (strSize > bufSize) {
        fprintf(stderr, "Error: Attempting to parse string longer than buffer, bytes out of range\n");
        return NULL;
    }

    char* str = malloc(sizeof(char) * strSize);

    int len = 0;
    while (len <= bufSize && len < strSize && (buf[len] != '\0' || buf[len] != EOF)) {
        str[len] = buf[len];
        len++;
        
        if (len == strSize - 1) break;
    }

    str[len++] = '\0';

    // reallocate string to trim the fat
    return realloc(str, len);
}

Block_t parse_Block(const uint8_t *buf, size_t bufSize) {
    if (5 > bufSize) {
        fprintf(stderr, "Error: Attempting to parse a Block_t, bytes out of range\n");
    }

    return (Block_t) {
        buf[0],
        parse_uint32(buf, bufSize, 1)
    };
}

// this assumes that the start of the buffer contains the start of
// this particular character data block
CharData_t parse_CharData(uint8_t *buf, size_t bufSize) {
    if (bufSize < 20) {
        fprintf(stderr, "Error: CharData buffer size mismatch, unable to parse CharData properly.\n");
    }
    // specification says each character is 4 bytes,
    // assuming this is to support utf32, but we only need utf8
    // so we can just cast directly to a char
    char id = (char) parse_uint32(buf, bufSize, 0);

    // x and y are in top-left
    uint16_t x = parse_uint16(buf, bufSize, 4);
    uint16_t y = parse_uint16(buf, bufSize, 6);

    uint16_t w = parse_uint16(buf, bufSize, 8);
    uint16_t h = parse_uint16(buf, bufSize, 10);
    uint8_t channel = buf[19];

    return (CharData_t) {
        .character = id,
        .x = x,
        .y = y,
        .width = w,
        .height = h,
        .channel = channel
    };
}

void read_bytes(uint8_t *buf, FILE *file, size_t amount) {
    if (fread(buf, sizeof(uint8_t), amount, file) != amount) {
        fprintf(stderr, "Error: byte index out of range.\n");
        return;
    }
}

int read_Header(uint8_t *buf, FILE *file, FontData_t *fontData) {
// Check file header
    read_bytes(buf, file, 4);

    if (buf[0] != 'B' || buf[1] != 'M' || buf[2] != 'F') {
        fprintf(stderr, "Error: Font data file does not contain BMF header.\n");
        return 0;
    }

    if (buf[3] == 3) {
        printf("Loading BMF font format version 3\n");
    } else {
        fprintf(stderr, "Error: Unsupported BMF format version: %u\n", buf[3]);
        return 0;
    }

    return 1;
}

// reads the binary font data file as defined in:
// https://www.angelcode.com/products/bmfont/doc/file_format.html#bin
void FontRenderer_loadData(char* fontPath, FontData_t *fontData) {
    FILE *filePtr = fopen(fontPath, "rb");
    uint8_t buf[128];
    size_t bufSize = (sizeof(buf) / sizeof(uint8_t));

    if (filePtr == NULL) {
        fprintf(stderr, "Error: loading font file: %s\n", fontPath);
        return;
    }

    if (!read_Header(buf, filePtr, fontData)) return;

    // Read & cache block types
    read_bytes(buf, filePtr, 5);

    // Read only the font name from block 1

    Block_t block1 = parse_Block(buf, bufSize);
    // fontName is 14 bytes ahead
    fseek(filePtr, 14, SEEK_CUR);
    uint32_t strLen = block1.size - 14;
    read_bytes(buf, filePtr, strLen);
    fontData->fontName = parse_string(buf, bufSize, strLen);
    fontData->nameLen = strlen(fontData->fontName);


    // skip over block 2
    read_bytes(buf, filePtr, 5);
    fseek(filePtr, parse_Block(buf, bufSize).size, SEEK_CUR);

    // read block 3 to get our texture
    read_bytes(buf, filePtr, 5);
    Block_t block3 = parse_Block(buf, bufSize);
    
    // read texture file name
    read_bytes(buf, filePtr, block3.size);
    char* texName = parse_string(buf, bufSize, block3.size);

    // open file and parse image using stb_image.h
    char texPath[128] = "assets/";
    strcat(texPath, texName);

    fontData->fontAtlas = DW_loadTexture(texPath);

    // load char data from block 4
    read_bytes(buf, filePtr, 5);
    Block_t block4 = parse_Block(buf, bufSize);
    
    // we know each unit of chardata is 20 bytes
    // So we can find how many chars we have
    size_t charCount = block4.size / 20;
    fontData->charCount = charCount;
    fontData->charData = (CharData_t*) malloc(charCount * sizeof(CharData_t));

    // read every char
    for (int i = 0; i < charCount; i++) {
        read_bytes(buf, filePtr, 20);
        CharData_t charData = parse_CharData(buf, bufSize);
        fontData->charData[i] = charData;
    }

    fclose(filePtr);

    printf("Loaded bitmap font: %s @ %s\n", fontData->fontName, texPath);
}

// Puts the verticies for a given char into the given vertex array
void CharData_genUV(FontData_t *fontData, int charIndex, Vertex_PT *buffer, size_t bufLen, size_t index) {
    CharData_t *charData = &fontData->charData[charIndex];
    float texW = (float) fontData->fontAtlas.width;
    float texH = (float) fontData->fontAtlas.height;
    if (index + 3 > bufLen) {
        fprintf(stderr, "Error: Generating font verticies, buffer too small\n");
        return;
    }

    
    float texLeft = (charData->x) / texW;
    float texRight = (charData->x + charData->width) / texW;

    // top in uv coords is 1.0
    float texTop = 1.0f - (charData->y / texH);
    float texBottom = 1.0f - ((charData->y + charData->height) / texH);
    
    
    // top left vertex
    buffer[index] = (Vertex_PT) { 0.0f, 0.0f, 0.0f,                             texLeft, texTop };
    // bottom left vertex
    buffer[index + 1] = (Vertex_PT) { 0.0f, charData->height, 0.0f,             texLeft, texBottom };
    // top right vertex
    buffer[index + 2] = (Vertex_PT) { charData->width, 0.0f, 0.0f,              texRight, texTop };
    // bottom right vertex
    buffer[index + 3] = (Vertex_PT) { charData->width, charData->height, 0.0f,  texRight, texBottom };
}

void FontRenderer_init(FontRenderer_t *font, Context_t *context, char* fontPath) {
    font->fontPath = fontPath;
    font->context = context;
    font->fontData = (FontData_t*) malloc(sizeof(FontData_t));
    FontRenderer_loadData(fontPath, font->fontData);
    // allocate and initialize a new static renderer
    size_t vertexCount = font->fontData->charCount * 4;

    // each "quad" for each char will have 4 verticies
    Vertex_PT fontVerticies[vertexCount];

    for (size_t i = 0; i < font->fontData->charCount; i++) {
        // every 4 verticies
        CharData_genUV(font->fontData, i, fontVerticies, vertexCount, i * 4);
    }

    VertexBuffer_t vb;
    VertexBuffer_init(&vb, VERTEX_FORMAT_PT, vertexCount, sizeof(fontVerticies), fontVerticies);

    // we are basically only drawing 2 triangles as a single quad
    uint32_t indicies[] = {
        0, 1, 2, 0, 2, 3
    };

    IndexBuffer_t ib;
    IndexBuffer_init(&ib, 6, sizeof(indicies), indicies);

    glGenVertexArrays(1, &font->vao);
    glBindVertexArray(font->vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_PT), (void*) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_PT), (void*) offsetof(Vertex_PT, uv));


}

void FontRenderer_bind(FontRenderer_t *font) {
    glUseProgram(font->shader);
    glBindVertexArray(font->vao);
    glBindBuffer(GL_ARRAY_BUFFER, font->vb.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, font->ib.ibo);
}

void FontData_free(FontData_t *fontData) {
    free(fontData->charData);
    fontData->charData = NULL;

    free(fontData->fontName);
    fontData->fontName = NULL;

    free(fontData);
    fontData = NULL;
}

void FontRenderer_free(FontRenderer_t *font) {
    // Renderer_free(font->renderer);    
    FontData_free(font->fontData);
    free(font);
}

void FontRenderer_drawChar(FontRenderer_t *font, char character) {
    if (character < 32 || character > 126) {
        character = '?';
    }

    FontRenderer_bind(font);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font->fontData->fontAtlas.texId);

    // glDrawElementsInstanced(GL_TRIANGLES, font->ib.indexCount, GL_UNSIGNED_INT, NULL, 10);
}


void FontRenderer_drawString(FontRenderer_t *font, char *text) {

}

uint32_t FontRenderer_getStringWidth(FontRenderer_t *font, char *text) {

}
 