#include "mainmenu.h"

#include "../globals.h"

// amount of menu options
#define SELECTION_MAX 2

char* titleText = "DeltaWing";
uint32_t titleWidth;
uint8_t selectionIndex = 1;


void MainMenu_init() {
    titleWidth = FontRenderer_getStringWidth(fontRenderer, titleText);
}

void MainMenu_tick() {

}

void MainMenu_render() {
    FontRenderer_bind(fontRenderer);
    FontRenderer_setColor(fontRenderer, (vec4) { 1.0f, 0.0f, 0.0f, 1.0f });
    FontRenderer_drawString(fontRenderer, titleText, 
        (DISPLAY_WIDTHF / 2.0f) - (titleWidth / 2.0f),
        (DISPLAY_HEIGHTF / 2.0f) - (fontRenderer->charHeight)
    );

    FontRenderer_setColor(fontRenderer, (vec4) { 1.0f, 1.0f, 1.0f, 1.0f });
    FontRenderer_drawString(fontRenderer, RELEASE_VERSION_STR, 2.0f, DISPLAY_HEIGHTF - fontRenderer->charHeight - 2.0f);
    FontRenderer_drawString(fontRenderer, ">",
        1.0f,
        (DISPLAY_HEIGHTF / 2.0f) + (fontRenderer->charHeight * selectionIndex)
    );

    FontRenderer_drawString(fontRenderer, "Play",
        14.0f,
        (DISPLAY_HEIGHTF / 2.0f) + (fontRenderer->charHeight)
    );
    FontRenderer_drawString(fontRenderer, "Exit",
        14.0f,
        (DISPLAY_HEIGHTF / 2.0f) + (fontRenderer->charHeight * 2.0f)
    );
}

void MainMenu_exit() {

}

void MainMenu_onKey(int key, int scancode, int action, int mods) {
    // switch-seption
    if (action == GLFW_PRESS) {
        switch (key)
        {
        case GLFW_KEY_W:
        case GLFW_KEY_UP:
            if (selectionIndex == 1) {
                selectionIndex = SELECTION_MAX;
            } else {
                selectionIndex--;
            }
            break;
        case GLFW_KEY_S:
        case GLFW_KEY_DOWN:
            if (selectionIndex == SELECTION_MAX) {
                selectionIndex = 1;
            } else {
                selectionIndex++;
            }
            break;
        case GLFW_KEY_ENTER:
            switch (selectionIndex) {
                case 1:
                
                    break;
                case 2:
                    DW_exitGame();
                    break;
            }
            break;
        }
    }
}

void MainMenu_onClick(int button, int action, int mods) {

}