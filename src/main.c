#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "renderer.h"
#include "objects.h"

#define DISPLAY_WIDTH 1280
#define DISPLAY_HEIGHT 720

typedef struct Input {
    uint32_t mouseX;
    uint32_t mouseY;
    uint32_t prevMouseX;
    uint32_t prevMouseY;
    bool mouseState[8];
} Input_t;

// Our global game state struct containers

GLFWwindow* window;
Input_t* input;
Context_t* context;
Renderer_t* renderer;

uint32_t fps;

// skidded from stackoverflow, uses mingw lib sys/time.h
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
    printf("Key: %d Pressed: %d\n", key, action);
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

int DW_initContext() {
    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    window = glfwCreateWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "DeltaWing", NULL, NULL);
    glfwMakeContextCurrent(window);
    // setup callbacks
    glfwSetErrorCallback(DW_errorCallback);
    glfwSetKeyCallback(window, DW_keyCallback);
    glfwSetMouseButtonCallback(window, DW_mouseButtonCallback);
    glfwSetCursorPosCallback(window, DW_cursorPosCallback);

    if (!window) {
        fprintf(stderr, "Unable to create GLFW window");
        glfwTerminate();
        return 1;
    }

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Couldn't load OpenGL\n");
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

    renderer = malloc(sizeof(Renderer_t));
    Renderer_init(renderer, context);
    Renderer_bind(renderer);

    // Initialize with zeroes
    input = calloc(1, sizeof(Input_t));
}

void DW_exitGame() {
    Renderer_free(renderer);
    free(renderer);
    free(context);
}

const float left = (DISPLAY_WIDTH / 2) - 200.0f;
const float right = (DISPLAY_WIDTH / 2) + 200.0f;
const float top = (DISPLAY_HEIGHT / 2) - 200.0f;
const float bottom = (DISPLAY_HEIGHT / 2) + 200.0f;

void DW_render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderer->primitive = GL_TRIANGLES;
    Renderer_begin(renderer);

    Renderer_addVertex(renderer, (Vertex_t) {
        { left, bottom, 0.0f },
        { 1.0f, 0.0f, 0.0f, 1.0f }
    });
    Renderer_addVertex(renderer, (Vertex_t) {
        { (right + left) / 2.0f, top, 0.0f },
        { 0.0f, 1.0f, 0.0f, 1.0f }
    });
    Renderer_addVertex(renderer, (Vertex_t) {
        { right, bottom, 0.0f },
        { 0.0f, 0.0f, 1.0f, 1.0f }
    });

    Renderer_end(renderer);
}

int main(int argc, char** argv) {
    if (DW_initContext())
        return 1;
    
    DW_initGame();

    int frames = 0;
    const uint64_t startTime = DW_currentTimeMillis();
    uint64_t lastFrame = 0;

    glfwShowWindow(window);
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    while (!glfwWindowShouldClose(window)) {
        uint64_t currentFrameTime = DW_currentTimeMillis() - startTime;
        
        DW_render();
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        printf("mx %d my %d\n", input->mouseX, input->mouseY);
        // FPS Counter (updated once every 1000ms)
        frames++;
        if (currentFrameTime - lastFrame >= 1000) {
            fps = frames;
            frames = 0;
            lastFrame = currentFrameTime;
            
            // buffer of 24 bytes gives us like a max of 5 or 6 digits for fps format idk
            char buf[24];
            snprintf(buf, 24, "DeltaWing FPS: %d", fps);
            glfwSetWindowTitle(window, buf);
        }
    }

    DW_exitGame();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
