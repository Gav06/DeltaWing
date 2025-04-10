#ifndef WORLD_H
#define WORLD_H

void World_init();

void World_tick();

void World_render();

void World_exit();

void World_onKey(int key, int scancode, int action, int mods);

void World_onClick(int button, int action, int mods);

#endif