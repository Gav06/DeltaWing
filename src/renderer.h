#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct {
    float x, y, z; // vertex coords
    float r, g, b, a; // rgba (normalized)
} Vertex;

typedef struct {
    GLuint vao;
    GLuint vbo;
    GLenum mode;
    Vertex* verticies; // dynamic array of verticies
    GLuint vertexCount;
    GLuint capacity;
    GLuint shaderProgram;
} Renderer;

/* Init tessellator */
void Renderer_init(Renderer* r);

void Renderer_free(Renderer* r);

void Renderer_genDefaultVAO(GLuint* vbo, GLuint* vao);

void Renderer_genStaticVBO(GLuint* vbo, float* verticies, GLsizei vertexCount, GLsizeiptr vertexSize);

GLuint Shader_createProgram(const char* vertexShader, const char* fragShader);

void Shader_checkSrcError(GLuint shader);

void Shader_checkProgError(GLuint program);
