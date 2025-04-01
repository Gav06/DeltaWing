#ifndef FONT_H
#define FONT_H

#include "renderer.h"

typedef struct FontRenderer {
    Renderer_t* renderer;
    char* fontFile;
} FontRenderer_t;



#endif