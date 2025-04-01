#include "engine.h"

bool Rect_isInside(Rect_t *rect, vec2 point) {
    return point[0] > rect->pos[0] 
        && point[0] < rect->pos[0] + rect->size[0]
        && point[1] > rect->pos[1]
        && point[1] < rect->pos[1] + rect->size[1];
}
