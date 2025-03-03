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
    size_t vertexCount;
    size_t capacity;
    GLuint shaderProgram;
} Tessellator;



/* Init tessellator */
void t_init(Tessellator* tes);

GLuint s_createProgram(const char* vertexShader, const char* fragShader);

void s_checkSrcError(GLuint shader);

void s_checkProgError(GLuint program);