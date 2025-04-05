#ifndef FONT_H
#define FONT_H

#include "renderer.h"

typedef struct CharData {
    char character;

    uint16_t x;
    uint16_t y;

    uint16_t width;
    uint16_t height;
    
    uint8_t channel;    
} CharData_t;

typedef struct FontData {
    // texture for our bitmap.
    // while this font format supports multiple textures, we will only use 1
    // for this implementation
    GLuint texture;
    
    uint32_t texWidth;
    uint32_t texHeight;

    size_t nameLen;
    char *fontName;

    size_t charCount;
    CharData_t *charData;
} FontData_t;

typedef struct FontRenderer {
    // null terminated
    char *fontPath;
    FontData_t *fontData;
    Renderer_t *renderer;
    Context_t *context;
} FontRenderer_t;

void FontRenderer_init(FontRenderer_t *font, Context_t *context, char* fontPath);

void FontRenderer_free(FontRenderer_t *font);

void FontRenderer_drawString(FontRenderer_t *font, char *text);

uint32_t FontRenderer_getStringWidth(FontRenderer_t *font, char *text);

#endif