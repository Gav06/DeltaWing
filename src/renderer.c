#include <string.h>
#include <stddef.h>

#include "renderer.h"

const char* defaultFragShader = 
    "#version 460                                   \n"
    "in vec4 vertexColor;                           \n"
    "out vec4 fragColor;                            \n"
    "layout (location = 0) uniform int toggle;      \n"
    "void main() {                                  \n"
    "    fragColor = vertexColor;                   \n"
    "}                                              \n";

const char* defaultVertShader =
    "#version 460                                   \n"
    "layout (location = 0) in vec3 aPos;            \n"
    "layout (location = 1) in vec4 aColor;          \n"
    "out vec4 vertexColor;                          \n"
    "layout (location = 2) uniform mat4 projection; \n"
    "void main() {                                  \n"
    "   gl_Position = projection * vec4(aPos, 1.0); \n"
    "   vertexColor = aColor;                       \n"
    "}                                              \n";

GLuint Shader_createProgram(const char* vertShader, const char* fragShader) {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertShader, NULL);
    glCompileShader(vs);
    Shader_checkSrcError(vs);
    
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragShader, NULL);
    glCompileShader(fs);
    Shader_checkSrcError(fs);

    GLuint program = glCreateProgram();
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

void printLog(GLuint object, GLsizei logLen, GLboolean isShader) {
    if (logLen <= 0) return;
    
    char* logBuf = malloc(logLen * sizeof(char));
    if (!logBuf) {
        printf("Failed to allocate memory for log\n");
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

void Shader_checkSrcError(GLuint shader) {
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
        printf("Shader compilation failed\n");
    }
}

void Shader_checkProgError(GLuint program) {
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
        printf("Program linking failed\n");
    }
}

void R_init(Renderer_t* r, GLuint projWidth, GLuint projHeight) {
    // use default shader (can be changed later)
    r->shader = Shader_createProgram(defaultVertShader, defaultFragShader);
    r->vertexCount = 0;
    // gl_triangles is our default draw
    r->primitive = GL_TRIANGLES;
    glm_ortho(0.0f, (float) projWidth, (float) projHeight, 0.0f, -1.0f, 0.0f, r->projection);
    glm_vec3_zero(r->camPos);

    glGenVertexArrays(1, &r->vao);
    glBindVertexArray(r->vao);

    glGenBuffers(1, &r->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTICIES * sizeof(Vertex_t), NULL, GL_DYNAMIC_DRAW);

    // location 0, 3 elements, size float, normalized false, stride 7 of floats (xyz rgba)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_t), (void*) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex_t), (void*) offsetof(Vertex_t, color));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    r->isBound = GL_FALSE;
}

void R_bind(Renderer_t* r) {
    glBindVertexArray(r->vao);
    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
    r->isBound = GL_TRUE;
}

void R_checkBinding(Renderer_t* r) {
    if (!r->isBound) {
        fprintf(stderr, "Buffers of current Renderer are not bound!\n");
        return;
    }
}

void R_free(Renderer_t* r) {
    glDeleteBuffers(1, &r->vbo);
    glDeleteVertexArrays(1, &r->vao);
    glDeleteProgram(r->shader);
}

void R_addVertex(Renderer_t* r, Vertex_t v) {
    R_checkBinding(r);

    if (r->vertexCount >= MAX_VERTICIES) {
        fprintf(stderr, "Vertex buffer full!\n");
        return;
    }

    r->vertexData[r->vertexCount] = v;
    r->vertexCount++;
}

void R_beginDraw(Renderer_t* r) {
    R_checkBinding(r);
    // clear vertex data using memset (disabled)
    // memset(r->vertexData, 0, r->vertexCount * sizeof(Vertex_t));
    r->vertexCount = 0;
}

void R_endDraw(Renderer_t* r) {
    // assuming this renderer is already bound and in-use
    R_checkBinding(r);

    // use shader
    glUseProgram(r->shader);
    // update uniforms before drawing
    glUniformMatrix4fv(2, 1, GL_FALSE, *r->projection);
    // pass vertex data to GPU
    glBufferSubData(GL_ARRAY_BUFFER, 0, r->vertexCount * sizeof(Vertex_t), r->vertexData);
    // draw call
    glDrawArrays(r->primitive, 0, r->vertexCount);
}