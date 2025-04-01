#ifndef RENDERER_H
#define RENDERER_H

#include <stdlib.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#define MAX_TRIANGLES 2048
#define MAX_VERTICIES MAX_TRIANGLES * 3

#define MAX_MATRIX_STACK_SIZE 127

/**
 * A stack data structure for matricies
 * Mainly used for the model/transformation matrix in our case,
 * Similar to the old glPushMatrix library functions
 */ 
typedef struct MatrixStack {
    CGLM_ALIGN_MAT mat4 array[MAX_MATRIX_STACK_SIZE];
    int top;
} MatrixStack_t;

typedef struct Vertex {
    vec3 pos;
    vec4 color;
} Vertex_t;

typedef struct Context {
    // delta time variable
    float partialTicks;
    // display size
    uint32_t displayWidth;
    uint32_t displayHeight;
    vec3 camPos;
    // matricies
    mat4 projectionMatrix;
    MatrixStack_t* matrixStack;
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


// Initializes our stack
void MatrixStack_init(MatrixStack_t* stack);

// Push a new layer to the stack
void MatrixStack_push(MatrixStack_t* stack, mat4 matrix);

// Pop the actively pushed layer
mat4* MatrixStack_pop(MatrixStack_t* stack);

// Get the current matrix on top of the stack
mat4* MatrixStack_peek(MatrixStack_t* stack);

bool MatrixStack_isFull(MatrixStack_t* stack);

bool MatrixStack_isEmpty(MatrixStack_t* stack);

// Pushes a copy of the current matrix on top
void DW_pushMatrix(MatrixStack_t* stack);
// pops
void DW_popMatrix(MatrixStack_t* stack);

void DW_translate(MatrixStack_t* stack, vec3 vector);

void DW_rotate(MatrixStack_t* stack, float angle, vec3 axis);

void Context_init(Context_t* context, uint32_t width, uint32_t height);

void Context_free(Context_t* context);

void Renderer_init(Renderer_t* renderer, Context_t* context);

void Renderer_bind(Renderer_t* renderer);

void Renderer_free(Renderer_t* renderer);

void Renderer_addVertex(Renderer_t* renderer, Vertex_t vertex);

void Renderer_begin(Renderer_t* renderer);

void Renderer_push(Renderer_t* renderer);

uint32_t Shader_createProgram(const char* vertexShader, const char* fragShader);

void Shader_checkSrcError(uint32_t shader);

void Shader_checkProgError(uint32_t program);

#endif