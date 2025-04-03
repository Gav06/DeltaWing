#include <stdio.h>
#include <string.h>

#include "renderer.h"
#include "font.h"

// our image loading library
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


typedef struct Block {
    uint8_t type;
    uint32_t size;
} Block_t;

// Some helper functions to turn bytes into ints


uint32_t parse_uint32(const uint8_t *buf, size_t index) {
    if (index + 4 > (sizeof(buf) /  sizeof(uint8_t))) {
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

int32_t parse_int32(const uint8_t *buf, size_t index) {
    if (index + 4 > (sizeof(buf) /  sizeof(uint8_t))) {
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

Block_t parse_Block(const uint8_t *buf) {
    return (Block_t) {
        buf[0],
        parse_uint32(buf, 1)
    };
}

// Reads byte-by-byte until a null terminator is reached, thanks stackoverflow
char* parse_string(const uint8_t *buf, size_t size) {
    char* str = malloc(sizeof(char) * size);

    int len = 0;
    while (len < size && (buf[len] != '\0' || buf[len] != EOF)) {
        str[len++] = buf[len];

        if (len == size - 1) break;
    }

    str[len++] = '\0';

    // reallocate string to trim the fat
    return realloc(str, len);
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

    if (filePtr == NULL) {
        fprintf(stderr, "Error: loading font file: %s\n", fontPath);
        return;
    }

    if (!read_Header(buf, filePtr, fontData)) return;

    // Read & cache block types
    read_bytes(buf, filePtr, 5);

    // Read only the font name from block 1

    Block_t block1 = parse_Block(buf);
    // fontName, offset 14
    fseek(filePtr, 14, SEEK_CUR);
    uint32_t strLen = block1.size - 14;
    read_bytes(buf, filePtr, strLen);
    fontData->fontName = parse_string(buf, strLen);


    // skip over block 2
    read_bytes(buf, filePtr, 5);
    fseek(filePtr, parse_Block(buf).size, SEEK_CUR);

    // read block 3 to get our texture
    read_bytes(buf, filePtr, 5);
    Block_t block3 = parse_Block(buf);
    
    // our parser will only support reading from 1 font texture, because that is enough for our needs
    read_bytes(buf, filePtr, block3.size);
    char* texPath = parse_string(buf, block3.size);
    printf("Texture0 path: %s\n", texPath);

    
    fclose(filePtr);
}

void FontRenderer_init(FontRenderer_t *font, Context_t *context, char* fontPath) {
    font->fontPath = fontPath;
    font->context = context;
    font->fontData = (FontData_t*) malloc(sizeof(FontData_t));
    FontRenderer_loadData(fontPath, font->fontData);
    // allocate and initialize a new static renderer
    font->renderer = (Renderer_t*) malloc(sizeof(Renderer_t));
    // Renderer_init(font->renderer, context, GL_STATIC_DRAW, );
}

void FontRenderer_free(FontRenderer_t *font) {
    Renderer_free(font->renderer);    

}


void FontRenderer_drawString(FontRenderer_t *font, char *text) {

}

uint32_t FontRenderer_getStringWidth(FontRenderer_t *font, char *text) {

}
 