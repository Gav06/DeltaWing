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


#ifdef DEFINE_GLOBALS

// defined in scenes.h
Scene_t Scene_MainMenu;
Scene_t Scene_World;

Input_t *input;
Context_t *context;
Renderer_t *dynRenderer;
FontRenderer_t *fontRenderer;
Scene_t *currentScene;

#else
extern Scene_t Scene_MainMenu;
extern Scene_t Scene_World;

extern Input_t *input;
extern Context_t *context;
extern Renderer_t *dynRenderer;
extern FontRenderer_t *fontRenderer;
extern Scene_t *currentScene;
#endif

void DW_exitGame();

void DW_setScene(Scene_t *scene);

#endif