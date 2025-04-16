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

// the camera position is the centerpoint of the screen
vec2 camPos = GLM_VEC2_ZERO;
float camZoom = 1.0f;

mat4 overlayMatrix;

void updateCamera() {
    float width = DISPLAY_WIDTHF / camZoom;
    float height = DISPLAY_HEIGHTF / camZoom;

    mat4 camOrtho;
    glm_ortho(
        camPos[0] - (width / 2.f),
        camPos[0] + (width / 2.f),
        camPos[1] - (height / 2.f),
        camPos[1] + (height / 2.f),
        -1.0f,
        0.0f,
        camOrtho
    );

    glm_mat4_copy(context->projectionMatrix, overlayMatrix);
    glm_mat4_copy(camOrtho, context->projectionMatrix);
}

void World_init() {
    player.init(&playerObj);
    player.reset();
}

void World_tick() {
    player.tick();
}

void World_render() {
    glClearColor(0.361f, 0.835f, 0.917f, 1.0f);

    // setup our camera matricies for the world
    updateCamera();
    player.render();

    // restore our orthogonal matrix for 2d overlay rendering
    glm_mat4_copy(overlayMatrix, context->projectionMatrix);

    FontRenderer_setColor(fontRenderer, GLM_VEC4_ONE);
    FontRenderer_drawString(fontRenderer, "Jump through the pipes", 2.0f, 2.0f);
}

void World_exit() {

}

void World_onKey(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        // we use vec2_copy to overwrite the gravity accelleration
        // because we want the player to jump up instantly
        glm_vec2_copy((vec2) { 0.0f, 12.5f }, playerObj.velocity);
    }
}

void World_onClick(int button, int action, int mods) {
    
}