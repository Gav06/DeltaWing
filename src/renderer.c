#include <string.h>
#include <stddef.h>

#include "renderer.h"

// our image loading library
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

ImageData_t ImageData_fromFile(FILE *file) {
    int width, height, channels;
    uint8_t *img = stbi_load_from_file(file, &width, &height, &channels, 0);
    if (img == NULL) {
        fprintf(stderr, "Error: Loading image from file.\n");
    }

    return (ImageData_t) {
        width,
        height,
        channels,
        img
    };
}

void ImageData_freeImage(ImageData_t* imageData) {
    stbi_image_free(imageData->image);
}

GLuint ImageData_toTexture(ImageData_t* imgData) {
    GLuint texId;

    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format;

    switch (imgData->channels)
    {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            fprintf(stderr, "Error: Unsupported number of texture channels: %d\n", imgData->channels);
            break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, imgData->width, imgData->height, 0, format, GL_UNSIGNED_BYTE, imgData->image);

    return texId;
}

/* 
The following functions are for interfacing with the
MatrixStack struct directly, but DW_pushMatrix & DW_popMatrix will
serve as a more abstracted form, similar to glPushMatrix & glPopMatrix
*/
void MatrixStack_init(MatrixStack_t *stack) {
    stack->top = -1;
}

void MatrixStack_push(MatrixStack_t *stack, mat4 matrix) {
    if (MatrixStack_isFull(stack)) {
        // Stack overflow case
        fprintf(stderr, "Error: Stack size exceeds limit of %d. (stack overflow)\n", MAX_MATRIX_STACK_SIZE);
        return;
    }

    glm_mat4_copy(matrix, stack->array[++stack->top]);
}

mat4* MatrixStack_pop(MatrixStack_t *stack) {
    if (MatrixStack_isEmpty(stack)) {
        fprintf(stderr, "Error: Attempted to pop empty stack. (stack underflow)\n");
        return NULL;
    }

    return &stack->array[stack->top--];
}

mat4* MatrixStack_peek(MatrixStack_t *stack) {
    if (MatrixStack_isEmpty(stack)) {
        fprintf(stderr, "Error: Attempted to peek empty stack.\n");
        return NULL;
    }

    return &stack->array[stack->top];
}

bool MatrixStack_isFull(MatrixStack_t *stack) {
    return stack->top >= MAX_MATRIX_STACK_SIZE - 1;
}

bool MatrixStack_isEmpty(MatrixStack_t *stack) {
    return stack->top == -1;
}

void MatrixStack_pushMatrix(MatrixStack_t *stack) {
    MatrixStack_push(stack, *MatrixStack_peek(stack));
}

void MatrixStack_popMatrix(MatrixStack_t *stack) {
    MatrixStack_pop(stack);
}

void MatrixStack_translate(MatrixStack_t *stack, vec3 vector) {
    glm_translate(*MatrixStack_peek(stack), vector);
}

void MatrixStack_rotate(MatrixStack_t *stack, float angle, vec3 axis) {
    glm_rotate(*MatrixStack_peek(stack), angle, axis);
}

const char *defaultVertShader =
    "#version 460 core                              \n"
    "layout (location = 0) in vec3 aPos;            \n"
    "layout (location = 1) in vec4 aColor;          \n"
    "out vec4 vertexColor;                          \n"
    "flat out vec3 fragCoord;"
    "layout (location = 0) uniform mat4 projection; \n"
    "layout (location = 1) uniform mat4 model;  \n"
    "void main() {                                  \n"
    "   fragCoord = aPos;                           \n"
    "   gl_Position = projection * model * vec4(aPos, 1.0); \n"
    "   vertexColor = aColor;                       \n"
    "}                                              \n";

const char *defaultFragShader = 
    "#version 460 core                              \n"
    "in vec4 vertexColor;                           \n"
    "in vec3 fragCoord;                             \n"
    "out vec4 fragColor;                            \n"
    "void main() {                                  \n"
    "    fragColor = vertexColor;                   \n"
    "}                                              \n";

uint32_t Shader_createProgram(const char *vertShader, const char *fragShader) {
    uint32_t vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertShader, NULL);
    glCompileShader(vs);
    Shader_checkSrcError(vs);
    
    uint32_t fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragShader, NULL);
    glCompileShader(fs);
    Shader_checkSrcError(fs);

    uint32_t program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    Shader_checkProgError(program);

    glDetachShader(program, vs);
    glDetachShader(program, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void printLog(uint32_t object, GLsizei logLen, GLboolean isShader) {
    if (logLen <= 0) return;
    
    char *logBuf = malloc(logLen * sizeof(char));
    if (!logBuf) {
        fprintf(stderr, "Error: Failed to allocate memory for log\n");
        return;
    }
    
    if (isShader) {
        glGetShaderInfoLog(object, logLen, NULL, logBuf);
    } else {
        glGetProgramInfoLog(object, logLen, NULL, logBuf);
    }
    
    printf("%s\n", logBuf);
    free(logBuf);
}

void Shader_checkSrcError(uint32_t shader) {
    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    GLint logLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
    
    // Always print log if there is one, even for success
    if (logLen > 0) {
        printf("Shader compilation %s:\n", success ? "succeeded" : "failed");
        printLog(shader, logLen, GL_TRUE);
    }
    
    if (!success) {
        fprintf(stderr, "Error: Shader compilation failed\n");
    }
}

void Shader_checkProgError(uint32_t program) {
    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    
    GLint logLen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
    
    // Always print log if there is one, even for success
    if (logLen > 0) {
        printf("Program linking %s:\n", success ? "succeeded" : "failed");
        printLog(program, logLen, GL_FALSE);
    }
    
    if (!success) {
        fprintf(stderr, "Error: Program linking failed\n");
    }
}

void Context_init(Context_t *c, uint32_t width, uint32_t height) {
    // default cam pos
    glm_vec3_zero(c->camPos);
    // display dimensions
    c->displayWidth = width;
    c->displayHeight = height;
    // Setup matricies & transformation matrix stack

    c->matrixStack = malloc(sizeof(MatrixStack_t));
    MatrixStack_init(c->matrixStack);
    mat4 transform;
    glm_mat4_identity(transform);
    // push our identity transformation
    MatrixStack_push(c->matrixStack, transform);
    
    glm_ortho(0.0f, (float) c->displayWidth, (float) c->displayHeight, 0.0f, -1.0f, 0.0f, c->projectionMatrix);
}

void Context_free(Context_t *context) {
    free(context->matrixStack);
    free(context);
    
    context->matrixStack = NULL;
    context = NULL;
}

void Renderer_init(Renderer_t *r, Context_t *c, GLenum bufferUsage, Vertex_t *verticies) {
    // our renderer will use the matricies from our render context
    // in the vertex shader
    r->context = c;
    r->shader = Shader_createProgram(defaultVertShader, defaultFragShader);
    r->vertexCount = 0;
    r->primitive = GL_TRIANGLES;

    glUseProgram(r->shader);
    glUseProgram(0);

    glGenVertexArrays(1, &r->vao);
    glBindVertexArray(r->vao);

    glGenBuffers(1, &r->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
    // We use an orphaned buffer if not using static draw
    if (bufferUsage != GL_STATIC_DRAW) {
        r->vertexData = malloc(sizeof(Vertex_t) * MAX_VERTICIES);
        glBufferData(GL_ARRAY_BUFFER, MAX_VERTICIES * sizeof(Vertex_t), NULL, bufferUsage);
    } else {
        glBufferData(GL_ARRAY_BUFFER, MAX_VERTICIES * sizeof(Vertex_t), verticies, bufferUsage);
    }

    // location 0, 3 elements, size float, normalized false, stride 7 of floats (xyz rgba)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_t), (void*) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex_t), (void*) offsetof(Vertex_t, color));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    r->isBound = GL_FALSE;
}

void Renderer_bind(Renderer_t *r) {
    glBindVertexArray(r->vao);
    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
    r->isBound = GL_TRUE;
}

bool Renderer_checkBound(Renderer_t *r) {
    if (!r->isBound) {
        fprintf(stderr, "Error: Buffers of current Renderer are not bound!\n");
        return true;
    }

    return false;
}

void Renderer_free(Renderer_t *r) {
    glDeleteBuffers(1, &r->vbo);
    glDeleteVertexArrays(1, &r->vao);
    glDeleteProgram(r->shader);
    free(r);
    r = NULL;
}

void Renderer_addVertex(Renderer_t *r, Vertex_t v) {
    if (Renderer_checkBound(r)) return;

    if (r->vertexCount >= MAX_VERTICIES) {
        fprintf(stderr, "Error: Vertex buffer full!\n");
        return;
    }

    r->vertexData[r->vertexCount] = v;
    r->vertexCount++;
}

void Renderer_beginDynamic(Renderer_t *r) {
    if (Renderer_checkBound(r)) return;
    // clear vertex data using memset (disabled)
    // memset(r->vertexData, 0, r->vertexCount * sizeof(Vertex_t));
    r->vertexCount = 0;
}

void Renderer_drawDynamic(Renderer_t *r) {
    // assuming this renderer is already bound and in-use
    if (Renderer_checkBound(r)) return;

    // use shader
    glUseProgram(r->shader);
    // pass matrix data
    glUniformMatrix4fv(0, 1, GL_FALSE, *r->context->projectionMatrix);
    glUniformMatrix4fv(1, 1, GL_FALSE, (const GLfloat*) MatrixStack_peek(r->context->matrixStack));
    // pass vertex data to GPU
    glBufferSubData(GL_ARRAY_BUFFER, 0, r->vertexCount * sizeof(Vertex_t), r->vertexData);
    // draw call
    glDrawArrays(r->primitive, 0, r->vertexCount);
}

// start is inclusive, end is 
void Renderer_drawStaticInterval(Renderer_t *r, uint32_t start, uint32_t amount) {
    if (Renderer_checkBound(r)) return;
}

void Renderer_drawStatic(Renderer_t *r) {
}