#ifndef GLOBALS_H
#define GLOBALS_H

#include "input.h"
#include "font.h"
#include "engine.h"

#define DISPLAY_WIDTH 1280
#define DISPLAY_HEIGHT 720

#define DISPLAY_WIDTHF 1280.0f
#define DISPLAY_HEIGHTF 720.0f

#define RELEASE_VERSION_STR "v0.1"

Input_t *input;
Context_t *context;
Renderer_t *dynRenderer;
FontRenderer_t *fontRenderer;
Scene_t *currentScene;

void DW_exitGame();

#endif