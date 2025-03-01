#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "renderer.h"

// skidded from stackoverflow <3
int64_t current_time_millis() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int64_t s1 = (int64_t)(time.tv_sec) * 1000;
  int64_t s2 = (time.tv_usec / 1000);
  return s1 + s2;
}

void check_glsl_prog_error(int program, int name) {
    int code = 0;
    glGetProgramiv(program, name, &code);
    if (code == 0) {
        char* logBuf = malloc(1024 * sizeof(char));
        glGetProgramInfoLog(program, 1024, NULL, logBuf);
        printf("%s\n", logBuf);
    }
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %d %s\n", error, description);
}

int main(void) {

    if (!glfwInit()) {
        return 1;
    }

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "DeltaWing", NULL, NULL);
    glfwMakeContextCurrent(window);

    if (!window) {
        fprintf(stderr, "Unable to create GLFW window");
        glfwTerminate();
        return 1;
    }

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Couldn't load OpenGL\n");
        return 1;
    }

    // float points[] = {
    //     -0.5f, -0.5f, 0.0f,
    //     0.0f, 0.5f, 0.0f,
    //     0.5f, -0.5f, 0.0f
    // };

    float points[] = {
        -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f
    };

    const int length = (sizeof(points) / sizeof(float));

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    const char* fragShader = 
            "#version 460                                   \n"
            "const vec2 resolution = vec2(1280.0, 720.0);   \n"
            "in vec4 gl_FragCoord;                          \n"
            "out vec4 diffuseColor;                         \n"
            "layout(location = 0) uniform float uTime;      \n"
            "vec3 hsbToRgb(float h, float s, float b) {     \n"
            "   vec3 rgb = clamp(abs(mod(h * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);\n"
            "   rgb = rgb * rgb * (3.0 - 2.0 * rgb);        \n"
            "   return b * mix(vec3(1.0), rgb, s);          \n"
            "}                                              \n"
            "void main() {                                  \n"
            "   float offset = gl_FragCoord.x / resolution.x;\n"
            "   float hue = mod(mod(uTime / 2000.0, 1.0) + offset, 1.0);\n"
            "   vec3 rgb = hsbToRgb(hue, 1.0, 1.0);         \n"
            "   diffuseColor = vec4(rgb, 1.0);              \n"
            "}                                              \n";
    
    const char* vertShader =
            "#version 460                                   \n"
            "in vec3 vertex_pos;                            \n"
            "void main() {                                  \n"
            "   gl_Position = vec4(vertex_pos, 1.0);        \n"
            "}                                              \n";

    GLuint shader_program = Shader_createProgram(vertShader, fragShader);

    check_glsl_prog_error(shader_program, GL_LINK_STATUS);

    int timeUniform = glGetUniformLocation(shader_program, "uTime");

    int frames = 0;
    const long long startTime = current_time_millis();
    long long lastFrame = 0;

    glfwShowWindow(window);
    while (!glfwWindowShouldClose(window)) {
        long long currentFrameTime = current_time_millis() - startTime;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);
        glUniform1f(timeUniform, (float) currentFrameTime);
        glBindVertexArray(vao);
        glDrawArrays(GL_QUADS, 0, 4);
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
    glDeleteProgram(shader_program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}