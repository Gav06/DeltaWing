#include "player.h"

#include "../util.h"


#define GRAVITY_ACCEL 9.81f

VertexBuffer_t *vb;
IndexBuffer_t *ib;
Renderer_t *playerRenderer;

GameObj_t *gameObj;

void Player_init(GameObj_t *gameObjIn) {
    gameObj = gameObjIn;

    vb = malloc(sizeof(VertexBuffer_t));
    ib = malloc(sizeof(IndexBuffer_t));
    size_t vSize = VertexFormat_sizeOf(VERTEX_FORMAT_PC);
    Vertex_PC verticies[] = {
        (Vertex_PC) { { -20.0f, -20.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f} },
        (Vertex_PC) { { 0.0f, 25.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, 
        (Vertex_PC) { { 20.0f, -20.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }
    };

    VertexBuffer_init(vb, vSize, 3, 3 * vSize, GL_STATIC_DRAW, verticies);
    uint32_t indicies[] = { 0, 1, 2 };
    IndexBuffer_init(ib, 3, sizeof(indicies), indicies);

    playerRenderer = malloc(sizeof(Renderer_t));
    Renderer_init(playerRenderer, context, VERTEX_FORMAT_PC, vb, ib);
}

void Player_reset() {
    glm_vec2_copy(GLM_VEC2_ZERO, gameObj->pos);
}

// we will store our player's previous velocity so the jump upwards is *somewhat* smooth
float prevYVel = 0.0f;

void Player_tick() {
    // apply gravity
    prevYVel = gameObj->velocity[1];
    if (gameObj->velocity[1] > -GRAVITY_ACCEL) {
        glm_vec2_add((vec2) { 0.f, -GRAVITY_ACCEL / (TARGET_TPS / 2) }, gameObj->velocity, gameObj->velocity);
    }

    float newX = gameObj->pos[0] + gameObj->velocity[0];
    float newY = gameObj->pos[1] + gameObj->velocity[1];
    glm_vec2_copy(gameObj->pos, gameObj->prevPos);
    glm_vec2_copy((vec2) { newX, newY }, gameObj->pos);
}


void Player_render() {
    MatrixStack_pushMatrix(context->matrixStack);

    float renderX = DW_lerp(gameObj->prevPos[0], gameObj->pos[0], context->partialTicks);
    float renderY = DW_lerp(gameObj->prevPos[1], gameObj->pos[1], context->partialTicks);

    // calculate our rotation angle
    float angle = -M_PI_2;

    float yVel = DW_lerp(prevYVel, gameObj->velocity[1], context->partialTicks);
    float angleAdd = fmax(fmin(yVel * 5.0f, 90.0f), -90.0f);
    angle += (angleAdd * (M_PI / 180));

    MatrixStack_translate(context->matrixStack, (vec3) { renderX, renderY, 0.0f });
    MatrixStack_rotate(context->matrixStack, angle, (vec3) { 0.0f, 0.0f, 1.0f });
    MatrixStack_translate(context->matrixStack, (vec3) { -renderX, -renderY, 0.0f });


    MatrixStack_translate(context->matrixStack, (vec3) { renderX, renderY, 0.0f });
    Renderer_bind(playerRenderer);
    Renderer_draw(playerRenderer);
    
    MatrixStack_popMatrix(context->matrixStack);
}

void Player_kill() {
    Renderer_free(playerRenderer);
    IndexBuffer_free(ib);
    VertexBuffer_free(vb);
}

