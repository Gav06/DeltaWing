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
typedef struct Texture {
    uint32_t width;
    uint32_t height;
    uint8_t channels;
    GLuint texId;
} Texture_t;

Texture_t DW_loadTexture(char* texPath);

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
    // pos, color
    VERTEX_FORMAT_PC,
    // pos, tex
    VERTEX_FORMAT_PT,
    // pos, color, tex
    VERTEX_FORMAT_PCT,

    VERTEX_FORMAT_TOTAL
} VertexFormat_e;

typedef struct {
    vec3 pos;
    vec4 color;
} Vertex_PC;

typedef struct {
    vec3 pos;
    vec2 uv;
} Vertex_PT;

typedef struct {
    vec3 pos;
    vec4 color;
    vec2 uv;
} Vertex_PCT;

size_t VertexFormat_sizeOf(VertexFormat_e format);

typedef struct VertexBuffer {
    // vertex buffer object
    GLuint vbo;
    // sizeof whichever vertex format we decide to go with
    size_t stride;
    // number of defined verticies in our buffer
    size_t vertexCount;
    // The maximum amount of verticies that can be held in the allocated buffer
    size_t bufferSize;
} VertexBuffer_t;

typedef struct IndexBuffer {
    // element buffer object
    GLuint ibo;
    size_t indexCount;
    uint32_t *indexData;
} IndexBuffer_t;

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
 * Renderer struct, supports dynamic and static rendering, as well
 * as different vertex formats.
 */
typedef struct Renderer {
    // This is usually GL_TRIANGLES
    GLenum primitive;
    GLuint shader;

    VertexFormat_e vertexFormat;
    VertexBuffer_t vb;
    IndexBuffer_t ib;
    GLuint vao;

    GLint projectionLoc;
    GLint modelLoc;
    GLint samplerLoc;
    Context_t *context;
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

uint32_t Shader_createProgram(const char *vertexShader, const char *fragShader);

void Shader_checkSrcError(uint32_t shader);

void Shader_checkProgError(uint32_t program);

void Shader_compileDefaultShaders();

void IndexBuffer_init(IndexBuffer_t *ib, size_t indexCount, size_t dataSize, uint32_t *indexData);

void IndexBuffer_free(IndexBuffer_t *ib);

void VertexBuffer_init(VertexBuffer_t *vb, size_t stride, size_t vertexCount, size_t bufferSize, GLenum usage, void *vertexData);

void VertexBuffer_free(VertexBuffer_t *vb);

bool Renderer_checkBound(Renderer_t *renderer);

void Renderer_bind(Renderer_t *renderer);

void Renderer_init(Renderer_t *renderer, Context_t *context, VertexFormat_e format, VertexBuffer_t vb, IndexBuffer_t ib);

void Renderer_drawIndexed(Renderer_t *renderer, int start, size_t size);

void Renderer_draw(Renderer_t *renderer);

void Renderer_free(Renderer_t *renderer);

#endif