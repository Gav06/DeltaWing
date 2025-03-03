#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "renderer.h"

#define DISPLAY_WIDTH 1280
#define DISPLAY_HEIGHT 720

GLFWwindow* window;

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
        // pos (xyz)        // color (rgba)
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
    };

    Tessellator tes;
    t_init(&tes);

    printf("Vertex capacity: %d\n", tes.capacity);

    const int length = (sizeof(points) / sizeof(float));

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // vertex attribute 0, for the xyz pos of verticies
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // location 0, size of 3, size float, normalized: false, stride: 7 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), NULL);

    // vertex attribute 1, for rgba of verticies
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 7 * sizeof(float), (void*) (sizeof(float) * 3));

    const char* fragShader = 
            "#version 330 core              \n"
            "in vec4 vertexColor;           \n"
            "out vec4 diffuseColor;         \n"
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

    GLuint shaderProgram = s_createProgram(vertShader, fragShader);

    int frames = 0;
    const long long startTime = current_time_millis();
    long long lastFrame = 0;

    glfwShowWindow(window);

    while (!glfwWindowShouldClose(window)) {
        long long currentFrameTime = current_time_millis() - startTime;
        runTick();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        glUseProgram(0);

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

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}