#ifndef FONT_H
#define FONT_H

#include "renderer.h"

// We only support standard ASCII glyphs, no unicode
#define GLYPH_FIRST 32
#define GLYPH_LAST 126
#define GLYPH_COUNT (GLYPH_LAST - GLYPH_FIRST +1)

typedef struct CharData {
    char character;

    uint16_t x;
    uint16_t y;

    uint16_t width;
    uint16_t height;
    
    uint8_t channel;

    vec2 uv;
    vec2 uvSize;
} CharData_t;

typedef struct FontData {
    Texture_t fontAtlas;

    size_t nameLen;
    char *fontName;

    size_t charCount;
    CharData_t *charData;
} FontData_t;


// intermediary struct for data that will be passed to the instancing VBO
typedef struct GlyphInstance {
    // position of the instanced glyph
    vec2 pos;
    // actual size of glyph
    vec2 size;
    // top left of UV space for glyph
    vec2 uv;
    // char width, height as UV
    vec2 uvSize;
    // advance to next char
    float advance;
} GlyphInstance_t;

// This is a different type of renderer,
// unlike Renderer_t, we will use instancing to
// draw chars for better performance, since our data will
// never change, just the positions and uvs being drawn
typedef struct FontRenderer {
    // null terminated string
    char *fontPath;
    // where the data from our font file is stored (on heap)
    FontData_t *fontData;
    // pointer to our rendering context, there is usually only 1 of these
    Context_t *context;

    // renderer variables
    GLuint vao;
    VertexBuffer_t *vb;
    VertexBuffer_t *instanceVb;
    IndexBuffer_t *ib;
    GLuint shader;

    // our glyph information, for our instancing vbo
    size_t instanceDataSize;
    GlyphInstance_t *instanceData;
} FontRenderer_t;

void FontRenderer_init(FontRenderer_t *font, Context_t *context, char* fontPath);

void FontRenderer_bind(FontRenderer_t *font);

void FontRenderer_free(FontRenderer_t *font);

void FontRenderer_drawString(FontRenderer_t *font, char *text, float renderX, float renderY);

size_t FontRenderer_getStringWidth(FontRenderer_t *font, char *text);

#endif