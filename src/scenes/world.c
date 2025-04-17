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

typedef struct {
    vec2 pos;
    float gap;
} Pipe_t;

Renderer_t *pipeRenderer;

VertexBuffer_t *pipeVB;
IndexBuffer_t *pipeIB;


Pipe_t pipes[5];
int pipeCount = 0;

int nextPipe = 0;
int pipeTickTimer = 0;

void genPipe() {
    float random = (float) rand() / RAND_MAX;
    Pipe_t pipe;

    glm_vec2_copy((vec2) { DISPLAY_WIDTHF / 2.0f, 0.0f } ,pipe.pos);
    pipe.gap = (random * 20.0f) + 10.0f;
    pipes[nextPipe] = pipe;
    if (pipeCount < 5) pipeCount++;
    
    if (nextPipe > 4) {
        nextPipe = 0;
    } else {
        nextPipe++;
    }
}

const float PIPE_WIDTH = 50.0f;
const float PIPE_HEIGHT = 400.0f;

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
    pipeTickTimer = -1;

    size_t vSize = VertexFormat_sizeOf(VERTEX_FORMAT_PC);
    Vertex_PC verticies[] = {
        (Vertex_PC) { {-PIPE_WIDTH / 2.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f, 1.0f } }, // left bottom
        (Vertex_PC) { {-PIPE_WIDTH / 2.0f, PIPE_HEIGHT, 0.0f}, { 1.0f, 0.0f, 1.0f, 1.0f} }, // left top
        (Vertex_PC) { {PIPE_WIDTH / 2.0f, PIPE_HEIGHT, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f }}, // right top
        (Vertex_PC) { {PIPE_WIDTH / 2.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f} } // right bottom
    };

    pipeVB = malloc(sizeof(VertexBuffer_t));
    VertexBuffer_init(pipeVB, vSize, 4, 4 * vSize, GL_STATIC_DRAW, verticies);

    uint32_t indicies[] = {
        0, 1, 2, 0, 2, 3
    };
    pipeIB = malloc(sizeof(IndexBuffer_t));
    IndexBuffer_init(pipeIB, 6, sizeof(indicies), indicies);

    pipeRenderer = malloc(sizeof(Renderer_t));
    Renderer_init(pipeRenderer, context, VERTEX_FORMAT_PC, pipeVB, pipeIB);
}

void World_tick() {
    player.tick();


    if (pipeTickTimer > 100 || pipeTickTimer < 0) {
        pipeTickTimer = 0;
        genPipe();
    }
    pipeTickTimer++;


    for (int i = 0; i < pipeCount; i++) {
        Pipe_t pipe = pipes[i];
        float newX = pipe.pos[0] - 10.0f;
        printf("pipeX %f newX %f \n", pipe.pos[0], newX);
        pipe.pos[0] = newX;

        // printf("moved pipe %d\n", i);
    }
}

void World_render() {
    glClearColor(0.361f, 0.835f, 0.917f, 1.0f);

    // setup our camera matricies for the world
    updateCamera();
    player.render();

    Renderer_bind(pipeRenderer);
    for (int i = 0; i < pipeCount; i++) {
        Pipe_t pipe = pipes[i];
        MatrixStack_pushMatrix(context->matrixStack);
        // printf("pipe %d xy %f %f\n", i, pipe.pos[0], pipe.pos[1]);
        MatrixStack_translate(context->matrixStack, (vec3) { pipe.pos[0], pipe.pos[1], 0.0f });
        Renderer_draw(pipeRenderer);
        MatrixStack_popMatrix(context->matrixStack);
    }

    // restore our orthogonal matrix for 2d overlay rendering
    glm_mat4_copy(overlayMatrix, context->projectionMatrix);

    FontRenderer_setColor(fontRenderer, GLM_VEC4_ONE);
    FontRenderer_drawString(fontRenderer, "Score: 0", 2.0f, 2.0f);
}

void World_exit() {
    VertexBuffer_free(pipeVB);
    IndexBuffer_free(pipeIB);
    Renderer_free(pipeRenderer);
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