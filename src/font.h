#ifndef FONT_H
#define FONT_H

#include "renderer.h"

typedef struct CharData {
    uint32_t x;
    uint32_t y;

    uint32_t width;
    uint32_t height;

    int32_t xAdvance;
    uint32_t page;
    
    char character;
} CharData_t;

typedef struct FontData {
    size_t nameLen;
    char *fontName;

    size_t dataLen;
    CharData_t *charData;

    // array of textures for the pages
    GLuint *textures;
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