#ifndef PLAYER_H
#define PLAYER_H

#include "../engine.h"

void Player_init(GameObj_t *gameObj);

void Player_reset();

void Player_tick();

void Player_render();

void Player_kill();

#endif