// Header file meant to move some of the input functions out of main.c for
// cohesiveness and organization

#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct Input {
    uint32_t mouseX;
    uint32_t mouseY;
    uint32_t prevMouseX;
    uint32_t prevMouseY;
    bool mouseState[8];
    uint8_t keyStates[349];
    uint32_t currentMods;
} Input_t;

bool DW_isKeyDown(Input_t* input, int32_t key);

#endif