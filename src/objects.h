#ifndef OBJECTS_H
#define OBJECTS_H

#include <cglm/cglm.h>

typedef struct Rect {
    vec2 pos;
    vec2 size;
} Rect_t;

typedef struct GameObj {
    vec2 pos;
    vec2 velocity;
} GameObj_t;

bool Rect_isInside(Rect_t* rect, vec2 point);

// typedef struct 

#endif