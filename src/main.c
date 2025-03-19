#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "renderer.h"
#include "objects.h"

#define DISPLAY_WIDTH 1280
#define DISPLAY_HEIGHT 720

GLFWwindow* window_p;
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
    // printf("Key: %d Pressed: %d\n", key, action);
}

void DW_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    // printf("MButton: %d Pressed: %d\n", button, action);
}

void DW_cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    // printf("MPos: xy %d %d\n", (int) xpos, (int) ypos);
}

int DW_initContext() {
    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    window_p = glfwCreateWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "DeltaWing", NULL, NULL);
    glfwMakeContextCurrent(window_p);
    // setup callbacks
    glfwSetErrorCallback(DW_errorCallback);
    glfwSetKeyCallback(window_p, DW_keyCallback);
    glfwSetMouseButtonCallback(window_p, DW_mouseButtonCallback);
    glfwSetCursorPosCallback(window_p, DW_cursorPosCallback);

    if (!window_p) {
        fprintf(stderr, "Unable to create GLFW window");
        glfwTerminate();
        return 1;
    }

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Couldn't load OpenGL\n");
        return 1;
    }

    return 0;
}

void DW_initGame() {
    // Setup our dynamic renderer & render context
    context = malloc(sizeof(Context_t));
    Context_init(context, DISPLAY_WIDTH, DISPLAY_HEIGHT);

    renderer = malloc(sizeof(Renderer_t));
    Renderer_init(renderer, context);
    Renderer_bind(renderer);

    renderer->primitive = GL_QUADS;

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

    Renderer_begin(renderer);

    Renderer_addVertex(renderer, (Vertex_t) {
        { left, top, 0.0f },
        { 0.0f, 1.0f, 0.0f, 1.0f }
    });
    Renderer_addVertex(renderer, (Vertex_t) {
        { left, bottom, 0.0f },
        { 1.0f, 0.0f, 0.0f, 1.0f }
    });
    Renderer_addVertex(renderer, (Vertex_t) {
        { right, bottom, 0.0f },
        { 1.0f, 1.0f, 0.0f, 1.0f }
    });
    Renderer_addVertex(renderer, (Vertex_t) {
        { right, top, 0.0f },
        { 0.0f, 0.0f, 1.0f, 1.0f }
    });
    
    Renderer_end(renderer);
}

int main(int argc, char** argv) {
    if (DW_initContext())
        return 1;

    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

    const char* version = glGetString(GL_VERSION);
    printf("OpenGL %s\n", version);
    
    int frames = 0;
    const uint64_t startTime = DW_currentTimeMillis();
    uint64_t lastFrame = 0;

    DW_initGame();
    glfwShowWindow(window_p);
    while (!glfwWindowShouldClose(window_p)) {
        uint64_t currentFrameTime = DW_currentTimeMillis() - startTime;
        
        DW_render();
        glfwSwapBuffers(window_p);
        glfwPollEvents();

        // FPS Counter (updated once every 1000ms)
        frames++;
        if (currentFrameTime - lastFrame >= 1000) {
            fps = frames;
            frames = 0;
            lastFrame = currentFrameTime;
            
            // buffer of 24 bytes gives us like a max of 5 or 6 digits for fps format idk
            char buf[24];
            snprintf(buf, 24, "DeltaWing FPS: %d", fps);
            glfwSetWindowTitle(window_p, buf);
        }
    }

    DW_exitGame();

    glfwDestroyWindow(window_p);
    glfwTerminate();
    return 0;
}