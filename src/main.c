#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "renderer.h"

#define DISPLAY_WIDTH 1280
#define DISPLAY_HEIGHT 720

GLFWwindow* window_p;
Renderer_t* renderer_p;

// skidded from stackoverflow, uses mingw lib sys/time.h
int64_t current_time_millis() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int64_t s1 = (int64_t)(time.tv_sec) * 1000;
  int64_t s2 = (time.tv_usec / 1000);
  return s1 + s2;
}

void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %d %s\n", error, description);
}

int initGL() {
    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    window_p = glfwCreateWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "DeltaWing", NULL, NULL);
    glfwMakeContextCurrent(window_p);
    glfwSetErrorCallback(errorCallback);

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

void initGame() {
    renderer_p = malloc(sizeof(Renderer_t));
    R_init(renderer_p);
}

void exitGame() {
    R_free(renderer_p);
    free(renderer_p);
}

void tick() {
    
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    R_beginDraw(renderer_p);

    R_addVertex(renderer_p, (Vertex_t) { 
        {-0.5f, -0.5f, 0.0f}, 
        {1.0f, 0.0f, 0.0f, 1.0f} 
    });
    R_addVertex(renderer_p, (Vertex_t) { 
        {0.0f, 0.5f, 0.0f}, 
        {0.0f, 1.0f, 0.0f, 1.0f} 
    });
    R_addVertex(renderer_p, (Vertex_t) { 
        {0.5f, -0.5f, 0.0f}, 
        {0.0f, 0.0f, 1.0f, 1.0f} 
    });

    R_endDraw(renderer_p);

}

int main() {
    if (initGL())
        return 1;

    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

    const char* version = glGetString(GL_VERSION);
    printf("OpenGL %s\n", version);
    
    int frames = 0;
    const long long startTime = current_time_millis();
    long long lastFrame = 0;

    initGame();
    glfwShowWindow(window_p);
    while (!glfwWindowShouldClose(window_p)) {
        long long currentFrameTime = current_time_millis() - startTime;

        tick();
        render();
        glfwSwapBuffers(window_p);
        glfwPollEvents();

        // FPS Counter
        frames++;
        if (currentFrameTime - lastFrame >= 1000) {
            printf("FPS: %d\n", frames);
            frames = 0;
            lastFrame = currentFrameTime;
        }
    }

    exitGame();

    glfwDestroyWindow(window_p);
    glfwTerminate();
    return 0;
}