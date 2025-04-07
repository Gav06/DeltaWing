#include <string.h>
#include <stddef.h>

#include "renderer.h"

// our image loading library
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

size_t VertexFormat_getSize(VertexFormat_e format) {
    if (format < 0 || format >= VERTEX_FORMAT_TOTAL) {
        fprintf(stderr, "Error: Attempted to get size of invalid vertex format.\n");
        return 0;
    }

    switch (format) {
    case VERTEX_FORMAT_PC: return sizeof(Vertex_PC);
    case VERTEX_FORMAT_PT: return sizeof(Vertex_PT);
    case VERTEX_FORMAT_PCT: return sizeof(Vertex_PCT);
    }
}

Texture_t DW_loadTexture(char* texPath) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels;

    stbi_set_flip_vertically_on_load(true);
    uint8_t *data = stbi_load(texPath, &width, &height, &channels, 0);

    if (data) {
        GLenum format = (channels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        fprintf(stderr, "Error: Loading image with stbi_load failed: %s\n", texPath);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        fprintf(stderr, "Error: GL error while generating texture: %d\n", err);
    }

    return (Texture_t) {
        .width = width,
        .height = height,
        .channels = channels,
        .texId = texture
    };
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

// VertexFormat: Position, Color
const char *Vert_PosColor =
    "#version 460 core                                      \n"
    "layout (location = 0) in vec3 aPos;                    \n"
    "layout (location = 1) in vec4 aColor;                  \n"
    "uniform mat4 projection;         \n"
    "uniform mat4 model;              \n"
    "out vec4 vertexColor;                                  \n"
    "out vec3 fragCoord;                                    \n"
    "void main() {                                          \n"
    "   gl_Position = projection * model * vec4(aPos, 1.0); \n"
    "   fragCoord = aPos;                                   \n"
    "   vertexColor = aColor;                               \n"
    "}                                                      \n";

const char *Frag_PosColor = 
    "#version 460 core                              \n"
    "in vec4 vertexColor;                           \n"
    "in vec3 fragCoord;                             \n"
    "out vec4 fragColor;                            \n"
    "void main() {                                  \n"
    "    fragColor = vertexColor;                   \n"
    "}                                              \n";

// VertexFormat: Position, Texture
const char *Vert_PosTex =
    "#version 460 core                                      \n"
    "layout (location = 0) in vec3 aPos;                    \n"
    "layout (location = 1) in vec2 aTex;                    \n"
    "out vec2 texCoord;                                      \n"
    "out vec3 fragCoord;                                    \n"
    "uniform mat4 projection;                               \n"
    "uniform mat4 model;                                    \n"
    "void main() {                                          \n"
    "   gl_Position = projection * model * vec4(aPos, 1.0); \n"
    "   fragCoord = aPos;                                   \n"
    "   texCoord = aTex;                                     \n"
    "}                                                      \n";

const char *Frag_PosTex = 
    "#version 460 core                              \n"
    "in vec2 texCoord;                              \n"
    "in vec3 fragCoord;                             \n"
    "uniform sampler2D textureIn;                   \n"
    "out vec4 fragColor;                            \n"
    "void main() {                                  \n"
    "   vec4 texelColor = texture(textureIn, texCoord);\n"
    "   fragColor = texelColor;                     \n"
    "}                                              \n";

// VertexFormat: Position, Color, Texture
const char *Vert_PosColorTex =
    "#version 460 core                              \n"
    "layout (location = 0) in vec3 aPos;            \n"
    "layout (location = 1) in vec4 aColor;          \n"
    "layout (location = 2) in vec2 aTex;            \n"
    "uniform mat4 projection; \n"
    "uniform mat4 model;      \n"
    "out vec2 texCoord;                              \n"
    "out vec4 vertexColor;                          \n"
    "out vec3 fragCoord;                            \n"
    "void main() {                                  \n"
    "   gl_Position = projection * model * vec4(aPos, 1.0); \n"
    "   fragCoord = aPos;                           \n"
    "   texCoord = aTex;                             \n"
    "   vertexColor = aColor;                       \n"
    "}                                              \n";

const char *Frag_PosColorTex = 
    "#version 460 core                              \n"
    "in vec2 texCoord;                               \n"
    "in vec4 vertexColor;                           \n"
    "in vec3 fragCoord;                             \n"
    "uniform sampler2D textureIn;\n"
    "out vec4 fragColor;                            \n"
    "void main() {                                  \n"
    "   fragColor = texture(textureIn, texCoord) * vertexColor; \n"
    "}                                              \n";

bool shadersCompiled = false;
GLuint Shader_defaultShaderPrograms_m[VERTEX_FORMAT_TOTAL];

void Shader_compileDefaultShaders() {
    if (shadersCompiled) {
        fprintf(stderr, "Error: Default shaders are already compiled.\n");
        return;
    }

    // Compile our shaders for each vertex format
    for (int i = 0; i < VERTEX_FORMAT_TOTAL; i++) {
        GLuint prog;
        switch (i)
        {
        case VERTEX_FORMAT_PC:
            prog = Shader_createProgram(Vert_PosColor, Frag_PosColor);
            break;
        case VERTEX_FORMAT_PT:
            prog = Shader_createProgram(Vert_PosTex, Frag_PosTex);
            break;
        case VERTEX_FORMAT_PCT:
            prog = Shader_createProgram(Vert_PosColorTex, Frag_PosColorTex);
            break;
        default:
            fprintf(stderr, "Error: Attempting to compile default shader for invalid vertex format.\n");
            break;
        }
        Shader_defaultShaderPrograms_m[i] = prog;
    }
    
}

void Context_init(Context_t *c, uint32_t width, uint32_t height) {
    // default cam pos
    glm_vec3_zero(c->camPos);
    // display dimensions
    c->displayWidth = width;
    c->displayHeight = height;
    
    // Setup matricies & transformation matrix stack
    glm_ortho(0.0f, (float) c->displayWidth, (float) c->displayHeight, 0.0f, -1.0f, 0.0f, c->projectionMatrix);

    mat4 transform;
    glm_mat4_identity(transform);
    
    // push our identity transformation
    c->matrixStack = malloc(sizeof(MatrixStack_t));
    MatrixStack_init(c->matrixStack);
    MatrixStack_push(c->matrixStack, transform);
}

void Context_free(Context_t *context) {
    free(context->matrixStack);
    free(context);
    
    context->matrixStack = NULL;
    context = NULL;
}

void Renderer_init(Renderer_t *renderer, Context_t *context, VertexFormat_e format, GLenum usage, size_t vertexCount, void *vertexBuffer) {
    renderer->context = context;
    renderer->shader = Shader_defaultShaderPrograms_m[format];
    // default primitive is triangle strip as quads, but this will be
    // replaced with an element buffer later on
    renderer->primitive = GL_TRIANGLE_STRIP;
    renderer->vertexFormat = format;
    renderer->vertexSize = VertexFormat_getSize(format);
    renderer->vertexCount = vertexCount;
    renderer->usage = usage;

    size_t bufferSize = vertexCount * renderer->vertexSize;

    // Create vao and vbo
    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glGenBuffers(1, &renderer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);

    if (renderer->usage == GL_STATIC_DRAW) {
        glBufferData(GL_ARRAY_BUFFER, bufferSize, vertexBuffer, usage);
    } else {
        // If usage is GL_STREAM_DRAW or GL_DYNAMIC_DRAW
        // (vertex data will be supplied later, in a just-in-time or immediate fashion)
        renderer->dynamicVertexBuffer = malloc(renderer->vertexSize * MAX_VERTICIES);
        // data will be passed later with glBufferSubData
        glBufferData(GL_ARRAY_BUFFER, MAX_VERTICIES * renderer->vertexSize, NULL, usage);
    }

    // attribute 0 will always be the position, consisting of 3 floats
    // across all of our vertex formats
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, renderer->vertexSize, (void*) 0);

    glEnableVertexAttribArray(1);
    switch (format) {
    case VERTEX_FORMAT_PC:
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, renderer->vertexSize, (void*) offsetof(Vertex_PC, color));
        break;
    case VERTEX_FORMAT_PT:
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, renderer->vertexSize, (void*) offsetof(Vertex_PT, uv));
        break;
    case VERTEX_FORMAT_PCT:
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, renderer->vertexSize, (void*) offsetof(Vertex_PCT, color));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, renderer->vertexSize, (void*) offsetof(Vertex_PCT, uv));
        break;
    default:
        fprintf(stderr, "Error: Attempted to create Renderer with unknown Vertex format.\n");
        break;
    }

    // setup some shader uniforms
    glUseProgram(renderer->shader);
    // set our projection matrix now because it doesn't change at the moment.
    // our transformation matrix will be set every frame though.
    // use texture unit 0 as default
    renderer->projectionLoc = glGetUniformLocation(renderer->shader, "projection");
    renderer->modelLoc = glGetUniformLocation(renderer->shader, "model");

    // if we use a vertex format the has tex
    if (format != VERTEX_FORMAT_PC) {
        renderer->samplerLoc = glGetUniformLocation(renderer->shader, "textureIn");
    }
    
    glUseProgram(0);
}

void Renderer_bind(Renderer_t *renderer) {
    glBindVertexArray(renderer->vao);
    
    if (renderer->useIndexBuffer) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->eab);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    }

    glUseProgram(renderer->shader);
}

void Renderer_drawIndexed(Renderer_t *renderer, int start, size_t size) {

    if (renderer->vertexFormat != VERTEX_FORMAT_PC) {
        // int at;
        // glGetIntegerv(GL_ACTIVE_TEXTURE, &at);
        glUniform1i(renderer->samplerLoc, 0);
    }
    
    glUniformMatrix4fv(renderer->projectionLoc, 1, GL_FALSE, (float*) &renderer->context->projectionMatrix);
    glUniformMatrix4fv(renderer->modelLoc, 1, GL_FALSE, (float*) MatrixStack_peek(renderer->context->matrixStack));

    if (renderer->usage != GL_STATIC_DRAW) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, renderer->vertexCount * renderer->vertexSize, renderer->dynamicVertexBuffer);
    }

    if (renderer->useIndexBuffer) {
        glDrawElements(renderer->primitive, size, GL_UNSIGNED_INT, NULL);
    } else {
        glDrawArrays(renderer->primitive, start, size);
    }
}

void Renderer_draw(Renderer_t *renderer) {
    Renderer_drawIndexed(renderer, 0, renderer->vertexCount);
}

void Renderer_free(Renderer_t *renderer) {
    if (renderer->usage != GL_STATIC_DRAW) {
        free(renderer->dynamicVertexBuffer);
        renderer->dynamicVertexBuffer = NULL;
    }

    free(renderer);    
    renderer = NULL;
}