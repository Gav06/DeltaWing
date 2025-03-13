#include "renderer.h"
#include <string.h>
#include <stddef.h>

const char* defaultFragShader = 
    "#version 460                                   \n"
    "in vec4 vertexColor;                           \n"
    "out vec4 fragColor;                            \n"
    "void main() {                                  \n"
    "    fragColor = vertexColor;                   \n"
    "}                                              \n";


const char* defaultVertShader =
    "#version 460                                   \n"
    "layout (location = 0) in vec3 aPos;            \n"
    "layout (location = 1) in vec4 aColor;          \n"
    "out vec4 vertexColor;                          \n"
    "void main() {                                  \n"
    "   gl_Position = vec4(aPos, 1.0);              \n"
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

void printLog(GLuint glenum) {
    char* logBuf = malloc(1024 * sizeof(char));
    glGetProgramInfoLog(glenum, 1024, NULL, logBuf);
    printf("%s\n", logBuf);
    free(logBuf);
}

void Shader_checkSrcError(GLuint shader) {
    int code = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &code);
    if (code == 0) {
        printLog(shader);
    }
}

void Shader_checkProgError(GLuint program) {
    int code = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &code);
    if (code == 0) {
        printLog(program);
    }
}

void R_init(Renderer_t* r) {
    // use default shader (can be changed later)
    r->shader = Shader_createProgram(defaultVertShader, defaultFragShader);
    r->vertexCount = 0;
    r->primitive = GL_TRIANGLES;

    glGenVertexArrays(1, &r->vao);
    glBindVertexArray(r->vao);

    glGenBuffers(1, &r->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTICIES * sizeof(Vertex_t), NULL, GL_DYNAMIC_DRAW);

    // location 0, 3 elements, size float, normalized false, stride 7 of floats (xyz rgba)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_t), (void*) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex_t), (void*) sizeof(vec3));
}

void R_free(Renderer_t* r) {
    glDeleteBuffers(1, &r->vbo);
    glDeleteVertexArrays(1, &r->vao);
    glDeleteProgram(r->shader);
}

void R_addVertex(Renderer_t* r, Vertex_t v) {
    if (r->vertexCount >= MAX_VERTICIES) return;

    r->vertexData[r->vertexCount] = v;
    r->vertexCount++;
}

void R_beginDraw(Renderer_t* r) {
    // clear vertex data using memset (disabled)
    // memset(r->vertexData, 0, r->vertexCount * sizeof(Vertex_t));
    r->vertexCount = 0;
}

void R_endDraw(Renderer_t* r) {
    glUseProgram(r->shader);
    glBindVertexArray(r->vao);
    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, r->vertexCount * sizeof(Vertex_t), r->vertexData);
    glDrawArrays(r->primitive, 0, r->vertexCount);
}

void R_print(Renderer_t* r) {
    printf("prim: %d\n", r->primitive);
    printf("vertexCount: %d\n", r->vertexCount);
}