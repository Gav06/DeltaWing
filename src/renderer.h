#include <stdlib.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#define MAX_TRIANGLES 2048
#define MAX_VERTICIES MAX_TRIANGLES * 3

typedef struct Vertex {
    vec3 pos;
    vec4 color;
} Vertex_t;

/**
 * Dynamic renderer struct, used for moving objects and things that will have their 
 * vertex data re-uploaded every frame
 */
typedef struct DynamicRenderer {
    GLuint shader;
    GLuint vao;
    GLuint vbo;
    Vertex_t vertexData[MAX_VERTICIES];
    GLuint vertexCount;
    GLenum primitive;
    mat4 projection;
    vec3 camPos;
    GLboolean isBound;
} Renderer_t;

void R_init(Renderer_t* renderer, GLuint width, GLuint height);

void R_bind(Renderer_t* renderer);

void R_free(Renderer_t* renderer);

void R_addVertex(Renderer_t* renderer, Vertex_t vertex);

void R_beginDraw(Renderer_t* renderer);

void R_endDraw(Renderer_t* renderer);

GLuint Shader_createProgram(const char* vertexShader, const char* fragShader);

void Shader_checkSrcError(GLuint shader);

void Shader_checkProgError(GLuint program);

