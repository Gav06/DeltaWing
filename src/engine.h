#ifndef ENGINE_H
#define ENGINE_H

#include <cglm/vec2.h>
#include "renderer.h"

typedef struct Scene {
    void (*init)(void);
    void (*tick)(void);
    void (*render)(Renderer_t*, Context_t*);
    void (*exit)(void);
} Scene_t;

typedef struct Rect {
    vec2 pos;
    vec2 size;
} Rect_t;

typedef struct GameObj {
    vec2 pos;
    vec2 velocity;
} GameObj_t;

// dynamic array for GameObjects to be added/removed at runtime in scenes
typedef struct GameObjArray {
    uint32_t size;
    GameObj_t *ptr;
} GameObjArray_t;

bool Rect_isInside(Rect_t *rect, vec2 point);

#endif