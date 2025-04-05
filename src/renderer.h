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

// Used for texture loading
typedef struct ImageData {
    int width;
    int height;
    int channels;
    uint8_t *image;
} ImageData_t;

ImageData_t ImageData_fromFile(FILE *file);

void ImageData_freeImage(ImageData_t* imageData);

GLuint ImageData_toTexture(ImageData_t* imageData);

/**
 * A stack data structure for matricies
 * Mainly used for the model/transformation matrix in our case,
 * Similar to the old glPushMatrix library functions
 */ 
typedef struct MatrixStack {
    CGLM_ALIGN_MAT mat4 array[MAX_MATRIX_STACK_SIZE];
    int top;
} MatrixStack_t;

typedef enum VertexFormat {
    VERTEX_FORMAT_P,
    VERTEX_FORMAT_PC,
    VERTEX_FORMAT_PT,
    VERTEX_FORMAT_PCT
} VertexFormat_e;

// Vertex attributes
typedef vec3 v_pos;
typedef vec4 v_color;
typedef vec2 v_uv;

typedef v_pos Vertex_P;

typedef struct {
    v_pos pos;
    v_color color;
} Vertex_PC;

typedef struct {
    v_pos pos;
    v_uv uv;
} Vertex_PT;

typedef struct {
    v_pos pos;
    v_color color;
    v_uv uv;
} Vertex_PCT;

typedef struct Vertex {
    v_pos pos;
    v_color color;
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
    MatrixStack_t *matrixStack;
} Context_t;

/**
 * Dynamic renderer struct, used for moving objects and things that will have their 
 * vertex data re-uploaded every frame
 */
typedef struct Renderer {
    GLboolean isBound;

    uint32_t shader;
    uint32_t vao;
    uint32_t vbo;
    
    uint32_t vertexCount;
    // This is usually GL_TRIANGLES
    GLenum primitive;

    Context_t *context;
    Vertex_t *vertexData;
} Renderer_t;


// Initializes our stack
void MatrixStack_init(MatrixStack_t *stack);

// Push a new layer to the stack
void MatrixStack_push(MatrixStack_t *stack, mat4 matrix);

// Pop the actively pushed layer
mat4* MatrixStack_pop(MatrixStack_t *stack);

// Get the current matrix on top of the stack
mat4* MatrixStack_peek(MatrixStack_t *stack);

bool MatrixStack_isFull(MatrixStack_t *stack);

bool MatrixStack_isEmpty(MatrixStack_t *stack);

// Pushes a COPY of the current matrix on top
void MatrixStack_pushMatrix(MatrixStack_t *stack);

void MatrixStack_popMatrix(MatrixStack_t *stack);

void MatrixStack_translate(MatrixStack_t *stack, vec3 vector);

void MatrixStack_rotate(MatrixStack_t *stack, float angle, vec3 axis);

void Context_init(Context_t *context, uint32_t width, uint32_t height);

void Context_free(Context_t *context);

// Note that verticies only needs to be specified for static draw
void Renderer_init(Renderer_t *renderer, Context_t *context, GLenum bufferUsage, Vertex_t *verticies);

void Renderer_bind(Renderer_t *renderer);

void Renderer_free(Renderer_t *renderer);

void Renderer_addVertex(Renderer_t *renderer, Vertex_t vertex);

void Renderer_beginDynamic(Renderer_t *renderer);

void Renderer_drawDynamic(Renderer_t *renderer);

void Renderer_drawStatic(Renderer_t *renderer);

void Renderer_drawStaticInterval(Renderer_t *renderer, uint32_t start, uint32_t count);

uint32_t Shader_createProgram(const char *vertexShader, const char *fragShader);

void Shader_checkSrcError(uint32_t shader);

void Shader_checkProgError(uint32_t program);

#endif