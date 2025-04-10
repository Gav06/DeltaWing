#include "world.h"

#include "../engine.h"
#include "../entities/player.h"


GameObj_t playerObj;
Entity_t player = {
    .init = Player_init,
    .reset = Player_reset,
    .tick = Player_tick,
    .render = Player_render,
    .kill = Player_kill
};


void World_init() {
    player.init(&playerObj);
    player.reset();
}

void World_tick() {
    player.tick();
}

void World_render() {
    player.render();
}

void World_exit() {

}

void World_onKey(int key, int scancode, int action, int mods) {

}

void World_onClick(int button, int action, int mods) {
    
}