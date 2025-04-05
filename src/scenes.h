#ifndef SCENES_H
#define SCENES_H

#include "engine.h"
#include "scenes/mainmenu.h"

Scene_t Scene_MainMenu = {
    .init = MainMenu_init,
    .tick = MainMenu_tick,
    .render = MainMenu_render,
    .exit = MainMenu_exit
};


#endif