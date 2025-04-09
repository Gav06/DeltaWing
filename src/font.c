#include <stdio.h>
#include <string.h>

#include "renderer.h"
#include "font.h"
#include "util.h"


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
CharData_t parse_CharData(uint8_t *buf, size_t bufSize, uint32_t texW, uint32_t texH) {
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

    float uW = w / (float) texW;
    float vH = (h / (float) texH);

    // tex left
    float u = x / (float) texW;
    // tex right
    // tex top
    float v = 1.0f - (y / (float) texH) - vH;
    // tex bottom

    
    CharData_t charData = {
        .character = id,
        .x = x,
        .y = y,
        .width = w,
        .height = h,
        .channel = channel,
        .uv = { u, v },
        .uvSize = { uW, vH }
    };

    return charData;
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
        // charData requires texture size for calculating UV coordinates
        CharData_t charData = parse_CharData(buf, bufSize, fontData->fontAtlas.width, fontData->fontAtlas.height);
        fontData->charData[i] = charData;
    }

    fclose(filePtr);

    printf("Loaded bitmap font: %s @ %s\n", fontData->fontName, texPath);
}

// Puts the data relative to the font atlas into the glyph instance. position is set later when rendering.
GlyphInstance_t CharData_genGlyphInstance(FontData_t *fontData, int charIndex) {
    CharData_t *charData = &fontData->charData[charIndex];

    GlyphInstance_t glyph;

    // size, pos is done later
    glm_vec2_copy(
        (vec2) { (float) charData->width, (float) charData->height }, 
        glyph.size
    );

    // uv data
    glm_vec2_copy(charData->uv, glyph.uv);
    glm_vec2_copy(charData->uvSize, glyph.uvSize);

    glyph.advance = charData->width;

    return glyph;
}

void FontRenderer_init(FontRenderer_t *font, Context_t *context, char* fontPath) {
    // setup struct
    font->fontPath = fontPath;
    font->context = context;
    // load chars and font data
    font->fontData = (FontData_t*) malloc(sizeof(FontData_t));
    FontRenderer_loadData(fontPath, font->fontData);

    // Create glyph instance data
    size_t instanceSize = sizeof(GlyphInstance_t);
    printf("charCount %lu\n", font->fontData->charCount);
    font->instanceDataSize = instanceSize * font->fontData->charCount;
    font->instanceData = malloc(font->instanceDataSize);

    for (size_t i = 0; i < font->fontData->charCount; i++) {
        font->instanceData[i] = CharData_genGlyphInstance(font->fontData, i);
    }

    // create render pipeline
    
    font->shader = Shader_createProgram(DW_loadSourceFile("assets/font.vert"), DW_loadSourceFile("assets/font.frag"));


    // setup our vao
    glGenVertexArrays(1, &font->vao);
    glBindVertexArray(font->vao);

    uint32_t indicies[] = {
        0, 1, 2, 0, 2, 3
    };

    font->ib = malloc(sizeof(IndexBuffer_t));
    IndexBuffer_init(font->ib, 6, sizeof(indicies), indicies);


    // setup usual vertex attributes
    Vertex_PT verticies[] = {
        { { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
        { { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
        { { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } }
    };

    font->vb = malloc(sizeof(VertexBuffer_t));
    VertexBuffer_init(font->vb, sizeof(Vertex_PT), 4, sizeof(verticies), GL_STATIC_DRAW, verticies);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_PT), (void*) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_PT), (void*) offsetof(Vertex_PT, uv));

    // Setup instance buffer attributes
    font->instanceVb = malloc(sizeof(VertexBuffer_t));
    VertexBuffer_init(font->instanceVb, instanceSize, 0, 0, GL_STREAM_DRAW, NULL);

    // instance position
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, instanceSize, (void*) NULL);
    glVertexAttribDivisor(2, 1);
    // instance size
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, instanceSize, (void*) offsetof(GlyphInstance_t, size));
    glVertexAttribDivisor(3, 1);

    // uv
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, instanceSize, (void*) offsetof(GlyphInstance_t, uv));
    glVertexAttribDivisor(4, 1);

    // uv size
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, instanceSize, (void*) offsetof(GlyphInstance_t, uvSize));
    glVertexAttribDivisor(5, 1);


    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void FontRenderer_bind(FontRenderer_t *font) {
    glUseProgram(font->shader);
    glBindVertexArray(font->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, font->ib->ibo);
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
    glDeleteVertexArrays(1, &font->vao);
    IndexBuffer_free(font->ib);
    free(font->ib);
    VertexBuffer_free(font->vb);
    free(font->instanceData);
    free(font);
}

void FontRenderer_drawString(FontRenderer_t *font, char *text, float renderX, float renderY) {
    FontRenderer_bind(font);

    size_t charCount = strlen(text);
    GlyphInstance_t bufData[charCount];
    size_t bufSize = sizeof(bufData);
    // setup instance buffer data
    float cursorAdvance = 0.0f;
    for (int i = 0; i < charCount; i++) {

        char c = text[i];
        if (c < GLYPH_FIRST || c > GLYPH_LAST) {
            c = '?';
        }

        int inst = (int) c - GLYPH_FIRST;
        // printf("getting instance data index %u for char %c (%u)\n", inst, c, c);
        bufData[i] = font->instanceData[inst];
        bufData[i].pos[0] = renderX + cursorAdvance;
        bufData[i].pos[1] = renderY;
        cursorAdvance += bufData[i].advance;

    }
    // pass buffer data
    glBindBuffer(GL_ARRAY_BUFFER, font->instanceVb->vbo);
    glBufferData(GL_ARRAY_BUFFER, bufSize, bufData, GL_STREAM_DRAW);


    // pass matricies
    glUniform1i(glGetUniformLocation(font->shader, "textureIn"), 0);
    glUniformMatrix4fv(glGetUniformLocation(font->shader, "projection"), 1, GL_FALSE, (float*) &font->context->projectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(font->shader, "model"), 1, GL_FALSE, (float*) MatrixStack_peek(font->context->matrixStack));

    // bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font->fontData->fontAtlas.texId);

    glDrawElementsInstanced(GL_TRIANGLES, font->ib->indexCount, GL_UNSIGNED_INT, NULL, charCount);
}

size_t FontRenderer_getStringWidth(FontRenderer_t *font, char *text) {
    size_t totalWidth = 0;

    for (int i = 0; i < strlen(text); i++) {
        totalWidth += font->instanceData[i - GLYPH_FIRST].advance;
    }

    return totalWidth;
}
 