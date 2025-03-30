#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "renderer.h"
#include "objects.h"
#include "input.h"


#define DISPLAY_WIDTH 1280
#define DISPLAY_HEIGHT 720

#define TARGET_TPS 60
#define MS_PER_TICK (1000 / TARGET_TPS)

// Our global game state struct containers

GLFWwindow* window;
Input_t* input;

Context_t* context;
Renderer_t* dynRenderer;

uint32_t fps;

int32_t cameraX = 0;
int32_t cameraY = 0;

// skidded from stackoverflow, requires gnu lib sys/time.h
int64_t DW_currentTimeMillis() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int64_t s1 = (int64_t)(time.tv_sec) * 1000;
  int64_t s2 = (time.tv_usec / 1000);
  return s1 + s2;
}

void DW_errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %d %s\n", error, description);
}

void DW_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key >= 32 && key <= 348) {
        input->keyStates[key] = action; 
        input->currentMods = mods;
    }
}

void DW_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (0 <= button <= 8) {
        input->mouseState[button] = action;
    }
}

void DW_cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    input->prevMouseX = input->mouseX;
    input->prevMouseY = input->mouseY;
    input->mouseX = (uint32_t) xpos;
    input->mouseY = (uint32_t) ypos;
}

int DW_initWindow() {
    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    window = glfwCreateWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "DeltaWing", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // setup callbacks
    glfwSetErrorCallback(DW_errorCallback);
    glfwSetKeyCallback(window, DW_keyCallback);
    glfwSetMouseButtonCallback(window, DW_mouseButtonCallback);
    glfwSetCursorPosCallback(window, DW_cursorPosCallback);

    if (!window) {
        fprintf(stderr, "Error: Unable to create GLFW window");
        glfwTerminate();
        return 1;
    }

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "Error: Couldn't load OpenGL\n");
        return 1;
    }

    const char* version = glGetString(GL_VERSION);
    printf("OpenGL %s\n", version);

    return 0;
}

void DW_initGame() {
    // Setup our dynamic renderer & render context
    context = malloc(sizeof(Context_t));
    Context_init(context, DISPLAY_WIDTH, DISPLAY_HEIGHT);

    dynRenderer = malloc(sizeof(Renderer_t));
    Renderer_init(dynRenderer, context);

    Renderer_bind(dynRenderer);
    dynRenderer->primitive = GL_TRIANGLE_STRIP;

    // Initialize with zeroes
    input = calloc(1, sizeof(Input_t));
}

void DW_exitGame() {
    // Free up vram and heap
    Renderer_free(dynRenderer);
    Context_free(context);
    free(input);
}

const float left = (DISPLAY_WIDTH / 2) - 200.0f;
const float right = (DISPLAY_WIDTH / 2) + 200.0f;
const float top = (DISPLAY_HEIGHT / 2) - 200.0f;
const float bottom = (DISPLAY_HEIGHT / 2) + 200.0f;

void DW_tick() {
    //update camera
    int moveX = 0;
    int moveY = 0;
    if (DW_isKeyDown(input, GLFW_KEY_A)) {
        moveX -= 10;
    }
    if (DW_isKeyDown(input, GLFW_KEY_D)) {
        moveX += 10;
    }
    if (DW_isKeyDown(input, GLFW_KEY_W)) {
        moveY -= 10;
    }
    if (DW_isKeyDown(input, GLFW_KEY_S)) {
        moveY += 10;
    }

    cameraX += moveX;
    cameraY += moveY;
}

void DW_render(float partialTicks) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // drawing a "quad" with triangle strip because GL_QUAD is deprecated, 
    // and doesn't even work on unix-like builds
	
    DW_pushMatrix(context->matrixStack);
    glm_translate(*MatrixStack_peek(context->matrixStack), (vec3) { cameraX, cameraY, 0.0f });
    glm_rotate(*MatrixStack_peek(context->matrixStack), glfwGetTime(), (vec3) { 0.0f, 0.0f, 1.0f });


    Renderer_begin(dynRenderer);

    Renderer_addVertex(dynRenderer, (Vertex_t) {
        { left, bottom, 0.0f },
        { 1.0f, 1.0f, 0.0f, 1.0f }
    });
    Renderer_addVertex(dynRenderer, (Vertex_t) {
        { left, top, 0.0f },
        { 1.0f, 0.0f, 0.0f, 1.0f }
    });
    Renderer_addVertex(dynRenderer, (Vertex_t) {
        { right, bottom, 0.0f },
        { 0.0f, 0.0f, 1.0f, 1.0f }
    });
    Renderer_addVertex(dynRenderer, (Vertex_t) {
        { right, top, 0.0f },
        { 0.0f, 1.0f, 0.0f, 1.0f }
    });

    Renderer_push(dynRenderer);

    DW_popMatrix(context->matrixStack);

    Renderer_begin(dynRenderer);

    Renderer_addVertex(dynRenderer, (Vertex_t) {
        { left, bottom, 0.0f },
        { 1.0f, 1.0f, 0.0f, 1.0f }
    });
    Renderer_addVertex(dynRenderer, (Vertex_t) {
        { left, top, 0.0f },
        { 1.0f, 0.0f, 0.0f, 1.0f }
    });
    Renderer_addVertex(dynRenderer, (Vertex_t) {
        { right, bottom, 0.0f },
        { 0.0f, 0.0f, 1.0f, 1.0f }
    });
    Renderer_addVertex(dynRenderer, (Vertex_t) {
        { right, top, 0.0f },
        { 0.0f, 1.0f, 0.0f, 1.0f }
    });

    Renderer_push(dynRenderer);
}

int main(int argc, char** argv) {
    if (DW_initWindow())
        return 1;
    
    DW_initGame();

    glfwShowWindow(window);
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

    uint64_t lastTime = DW_currentTimeMillis();
    uint64_t accumulator = 0;
    uint64_t frameStart, deltaTime;
    uint32_t ticks = 0;

    // Game loop
    while (!glfwWindowShouldClose(window)) {

        // uint64_t currentFrameTime = DW_currentTimeMillis() - startTime;
        frameStart = DW_currentTimeMillis();
        deltaTime = frameStart - lastTime;
        lastTime = frameStart;
        accumulator += deltaTime;

        while (accumulator >= MS_PER_TICK) {
            DW_tick();
            
            ticks++;
            accumulator -= MS_PER_TICK;
        }

        const float partialTicks = (float) accumulator / MS_PER_TICK;
        
        DW_render(partialTicks);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // This must be called before destroying our context because
    // We need some GL functions to free our VRAM
    DW_exitGame();

    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
