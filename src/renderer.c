#include "renderer.h"

// s_ is for shaders
// t_ is for tessellator

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

void Shader_checkSrcError(GLuint shader) {
    int code = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &code);
    if (code == 0) {
        char* logBuf = malloc(1024 * sizeof(char));
        glGetShaderInfoLog(shader, 1024, NULL, logBuf);
        printf("%s\n", logBuf);
    }
}

void Shader_checkProgError(GLuint program) {
    int code = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &code);
    if (code == 0) {
        char* logBuf = malloc(1024 * sizeof(char));
        glGetProgramInfoLog(program, 1024, NULL, logBuf);
        printf("%s\n", logBuf);
    }
}

void Renderer_init(Renderer* r) {
    // TODO: actually implement this shi
    // r->vbo = Renderer_genStaticVBO()
    // r->vao = Renderer_genDefaultVAO();
    r->mode = 0;
    r->verticies = (Vertex*) malloc(r->capacity * sizeof(Vertex));
    r->capacity = 1024;
    r->vertexCount = 0;
    r->shaderProgram = 0;
}

void Renderer_free(Renderer* tes) {
    
}

void Renderer_genDefaultVAO(GLuint* vbo, GLuint* vao) {
    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    // vertex attribute 0, for the xyz pos of verticies
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    // location 0, size of 3, size float, normalized: false, stride: 7 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), NULL);

    // vertex attribute 1, for rgba of verticies
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 7 * sizeof(float), (void*) (sizeof(float) * 3));
}
    
// each vertex should be defined as x, y, z, r, g, b, a (stride of 7 floats)
void Renderer_genStaticVBO(GLuint* vbo, float* verticies, GLsizei vertexCount, GLsizeiptr vertexSize) {
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexCount * vertexSize, verticies, GL_STATIC_DRAW);
}

/** 

const char* rainbowShader = 
"#version 460                                   \n"
"const vec2 resolution = vec2(1280.0, 720.0);   \n"
"in vec4 gl_FragCoord;                          \n"
"out vec4 diffuseColor;                         \n"
"layout(location = 0) uniform float uTime;      \n"
"vec3 hsbToRgb(float h, float s, float b) {     \n"
"   vec3 rgb = clamp(abs(mod(h * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);\n"
"   rgb = rgb * rgb * (3.0 - 2.0 * rgb);        \n"
"   return b * mix(vec3(1.0), rgb, s);          \n"
"}                                              \n"
"void main() {                                  \n"
"   float offset = gl_FragCoord.x / resolution.x;\n"
"   float hue = mod(mod(uTime / 2000.0, 1.0) + offset, 1.0);\n"
"   vec3 rgb = hsbToRgb(hue, 1.0, 1.0);         \n"
"   diffuseColor = vec4(rgb, 1.0);              \n"
"}                                              \n";

*/
