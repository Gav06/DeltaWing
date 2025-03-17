#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "renderer.h"
#include "objects.h"

#define DISPLAY_WIDTH 1280
#define DISPLAY_HEIGHT 720

GLFWwindow* window_ptr;
Renderer_t* renderer_p;

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
    printf("Key: %s Pressed: %d\n", glfwGetKeyName(key, scancode), action);
}

void DW_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    printf("MButton: %d Pressed: %d\n", button, action);
}

void DW_cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    printf("MPos: xy %.0f %.0f\n", xpos, ypos);
}

int DW_initContext() {
    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    window_ptr = glfwCreateWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "DeltaWing", NULL, NULL);
    glfwMakeContextCurrent(window_ptr);
    // setup callbacks
    glfwSetErrorCallback(DW_errorCallback);
    glfwSetKeyCallback(window_ptr, DW_keyCallback);
    glfwSetMouseButtonCallback(window_ptr, DW_mouseButtonCallback);
    glfwSetCursorPosCallback(window_ptr, DW_cursorPosCallback);

    if (!window_ptr) {
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
    // Setup our dynamic renderer
    renderer_p = malloc(sizeof(Renderer_t));
    R_init(renderer_p, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    R_bind(renderer_p);


}

void DW_exitGame() {
    R_free(renderer_p);
    free(renderer_p);
}

void DW_render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    R_beginDraw(renderer_p);

    R_addVertex(renderer_p, (Vertex_t) { 
        {0.0f, 720.0f, 0.0f}, 
        {1.0f, 0.0f, 0.0f, 1.0f} 
    });
    R_addVertex(renderer_p, (Vertex_t) { 
        {640.0f, 0.0f, 0.0f}, 
        {0.0f, 1.0f, 0.0f, 1.0f} 
    });
    R_addVertex(renderer_p, (Vertex_t) { 
        {1280.0f, 720.0f, 0.0f}, 
        {0.0f, 0.0f, 1.0f, 1.0f} 
    });

    R_endDraw(renderer_p);

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
    glfwShowWindow(window_ptr);
    while (!glfwWindowShouldClose(window_ptr)) {
        uint64_t currentFrameTime = DW_currentTimeMillis() - startTime;
        
        DW_render();

        glfwSwapBuffers(window_ptr);
        glfwPollEvents();

        // FPS Counter (updated once every 1000ms)
        frames++;
        if (currentFrameTime - lastFrame >= 1000) {
            fps = frames;
            frames = 0;
            lastFrame = currentFrameTime;

            char buf[24];
            snprintf(buf, 24, "DeltaWing FPS: %d", fps);
            glfwSetWindowTitle(window_ptr, buf);
        }
    }

    DW_exitGame();

    glfwDestroyWindow(window_ptr);
    glfwTerminate();
    return 0;
}