#ifndef MAINMENU_H
#define MAINMENU_H

void MainMenu_init();

void MainMenu_tick();

void MainMenu_render();

void MainMenu_exit();

void MainMenu_onKey(int key, int scancode, int action, int mods);

void MainMenu_onClick(int button, int action, int mods);

#endif