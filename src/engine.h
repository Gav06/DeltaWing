#ifndef ENGINE_H
#define ENGINE_H

#include <cglm/vec2.h>
#include "renderer.h"

typedef struct {
    void (*init)(void);
    void (*tick)(void);
    void (*render)(void);
    void (*exit)(void);
    void (*onKey)(int key, int scancode, int action, int mods);
    void (*onClick)(int button, int action, int mods);
} Scene_t;

typedef struct {
    vec2 pos;
    vec2 size;
} Rect_t;

typedef struct {
    vec2 pos;
    vec2 velocity;
} GameObj_t;

// dynamic array for GameObjects to be added/removed at runtime in scenes
typedef struct {
    uint32_t size;
    GameObj_t *ptr;
} GameObjArray_t;

bool Rect_isInside(Rect_t *rect, vec2 point);

#endif