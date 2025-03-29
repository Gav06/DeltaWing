#include "input.h"
#include <GLFW/glfw3.h>


bool DW_isKeyDown(Input_t* input, int32_t key) {
    if (key < 32 || key > 348) {
        return false;
    }

    return input->keyStates[key] != GLFW_RELEASE;
}