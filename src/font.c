#include "font.h"

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

} FontData_t;

void FontRenderer_init(FontRenderer_t* font) {

}

void FontRenderer_free(FontRenderer_t* font) {

}

void FontRenderer_drawString(FontRenderer_t* font, char* text) {

}

uint32_t FontRenderer_getStringWidth(FontRenderer_t* font, char* text) {

}
