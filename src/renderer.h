#ifndef RENDERER_H
#define RENDERER_H

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

typedef struct Context {
    vec3 camPos;
    uint32_t displayWidth;
    uint32_t displayHeight;
    mat4 projection;
    mat4 transformation;
} Context_t;

/**
 * Dynamic renderer struct, used for moving objects and things that will have their 
 * vertex data re-uploaded every frame
 */
typedef struct DynamicRenderer {
    uint32_t shader;
    uint32_t vao;
    uint32_t vbo;
    uint32_t vertexCount;
    GLenum primitive;
    vec3 camPos;
    GLboolean isBound;
    Context_t* context;
    Vertex_t vertexData[MAX_VERTICIES];
} Renderer_t;

void Context_init(Context_t* context, uint32_t width, uint32_t height);

void Renderer_init(Renderer_t* renderer, Context_t* context);

void Renderer_bind(Renderer_t* renderer);

void Renderer_free(Renderer_t* renderer);

void Renderer_addVertex(Renderer_t* renderer, Vertex_t vertex);

void Renderer_begin(Renderer_t* renderer);

void Renderer_end(Renderer_t* renderer);

uint32_t Shader_createProgram(const char* vertexShader, const char* fragShader);

void Shader_checkSrcError(uint32_t shader);

void Shader_checkProgError(uint32_t program);

#endif