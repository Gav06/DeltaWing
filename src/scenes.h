#ifndef SCENES_H
#define SCENES_H

#include "scenes/mainmenu.h"
#include "scenes/world.h"

Scene_t Scene_MainMenu = {
    .init = MainMenu_init,
    .tick = MainMenu_tick,
    .render = MainMenu_render,
    .exit = MainMenu_exit,
    .onKey = MainMenu_onKey,
    .onClick = MainMenu_onClick
};

Scene_t Scene_World = {
    .init = World_init,
    .tick = World_tick,
    .render = World_render,
    .exit = World_exit,
    .onKey = World_onKey,
    .onClick = World_onClick
};


#endif