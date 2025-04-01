#ifndef FONT_H
#define FONT_H

#include "renderer.h"

typedef struct FontRenderer {
    Renderer_t *renderer;
    char *fontPath;
} FontRenderer_t;

void FontRenderer_init(FontRenderer_t *font);

void FontRenderer_free(FontRenderer_t *font);

void FontRenderer_drawString(FontRenderer_t *font, char *text);

uint32_t FontRenderer_getStringWidth(FontRenderer_t *font, char *text);

#endif