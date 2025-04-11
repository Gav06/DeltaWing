#include "player.h"

#include "../util.h"

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
        (Vertex_PC) { { -20.0f, 20.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f} },
        (Vertex_PC) { { 0.0f, -20.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, 
        (Vertex_PC) { { 20.0f, 20.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
    };

    VertexBuffer_init(vb, vSize, 3, 3 * vSize, GL_STATIC_DRAW, verticies);
    uint32_t indicies[] = { 0, 1, 2 };
    IndexBuffer_init(ib, 3, sizeof(indicies), indicies);

    playerRenderer = malloc(sizeof(Renderer_t));
    Renderer_init(playerRenderer, context, VERTEX_FORMAT_PC, vb, ib);
}

void Player_reset() {
    glm_vec2_copy((vec2) { DISPLAY_WIDTHF / 2.0f, DISPLAY_HEIGHTF / 2.0f }, gameObj->pos);
}

void Player_tick() {
    float velX = 0.0f;
    float velY = 0.0f;

    if (DW_isKeyDown(input, GLFW_KEY_W)) velY -= 10.0f;
    if (DW_isKeyDown(input, GLFW_KEY_S)) velY += 10.0f;
    if (DW_isKeyDown(input, GLFW_KEY_A)) velX -= 10.0f;
    if (DW_isKeyDown(input, GLFW_KEY_D)) velX += 10.0f;
    glm_vec2_copy((vec2) { velX, velY}, gameObj->velocity);



    float newX = gameObj->pos[0] + gameObj->velocity[0];
    float newY = gameObj->pos[1] + gameObj->velocity[1];
    glm_vec2_copy(gameObj->pos, gameObj->prevPos);
    glm_vec2_copy((vec2) { newX, newY }, gameObj->pos);
}

void Player_render() {
    MatrixStack_pushMatrix(context->matrixStack);

    float renderX = DW_lerp(gameObj->prevPos[0], gameObj->pos[0], context->partialTicks);
    float renderY = DW_lerp(gameObj->prevPos[1], gameObj->pos[1], context->partialTicks);

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
