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
    Texture_t texture;

    size_t nameLen;
    char *fontName;

    size_t charCount;
    CharData_t *charData;
} FontData_t;

// This is a different type of renderer,
// unlike Renderer_t, we will use instancing to
// draw chars for better performance, since our data will
// never change, just the positions and uvs being drawn
typedef struct FontRenderer {
    // null terminated string
    char *fontPath;
    FontData_t *fontData;
    Context_t *context;

    // renderer variables
    GLuint vao;
    VertexBuffer_t vb;
    IndexBuffer_t ib;
    GLuint shader;
} FontRenderer_t;

void FontRenderer_init(FontRenderer_t *font, Context_t *context, char* fontPath);

void FontRenderer_bind(FontRenderer_t *font);

void FontRenderer_free(FontRenderer_t *font);

void FontRenderer_drawChar(FontRenderer_t *font, char character);

void FontRenderer_drawString(FontRenderer_t *font, char *text);

uint32_t FontRenderer_getStringWidth(FontRenderer_t *font, char *text);

#endif