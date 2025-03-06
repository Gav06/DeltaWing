#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "renderer.h"

#define DISPLAY_WIDTH 1280
#define DISPLAY_HEIGHT 720

GLFWwindow* window;

const char* fragShader = 
    "#version 330 core              \n"
    "layout (location = 0) out vec4 diffuseColor;         \n"
    "in vec4 vertexColor;           \n"
    "void main() {                  \n"
    "    diffuseColor = vertexColor;\n"
    "}                              \n";

const char* vertShader =
    "#version 460                                   \n"
    "layout (location = 0) in vec3 aPos;            \n"
    "layout (location = 1) in vec4 aColor;          \n"
    "out vec4 vertexColor;                          \n"
    "void main() {                                  \n"
    "   gl_Position = vec4(aPos, 1.0);              \n"
    "   vertexColor = aColor;                       \n"
    "}                                              \n";


// skidded from stackoverflow
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

    window = glfwCreateWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "DeltaWing", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetErrorCallback(errorCallback);

    if (!window) {
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

void runTick() {

}

int main() {
    if (initGL())
        return 1;

    float points[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
    };

    // Renderer tes;
    // Renderer_init(&tes);

    // GLuint vbo = 0;
    // Renderer_genStaticVBO(&vbo, points, 4, sizeof(float) * 7);

    // GLuint vao = 0;
    // Renderer_genDefaultVAO(&vbo, &vao);

    // GLuint shaderProgram = Shader_createProgram(vertShader, fragShader);

    // FPS calculations
    int frames = 0;
    const long long startTime = current_time_millis();
    long long lastFrame = 0;


    // glUseProgram(shaderProgram);
    // glBindVertexArray(vao);

    glfwShowWindow(window);
    while (!glfwWindowShouldClose(window)) {
        long long currentFrameTime = current_time_millis() - startTime;
        runTick();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // glUseProgram(shaderProgram);
        // glBindVertexArray(vao);
        // glDrawArrays(GL_QUADS, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
        
        
        // FPS Counter
        frames++;
        if (currentFrameTime - lastFrame >= 1000) {
            printf("FPS: %d\n", frames);
            frames = 0;
            lastFrame = currentFrameTime;
        }

    }

    // glUseProgram(0);
    // glBindVertexArray(0);

    // glDeleteVertexArrays(1, &vao);
    // glDeleteBuffers(1, &vbo);
    // glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}