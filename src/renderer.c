#include "renderer.h"

GLuint s_createProgram(const char* vertShader, const char* fragShader) {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertShader, NULL);
    glCompileShader(vs);

    s_checkSrcError(vs);
    
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragShader, NULL);
    glCompileShader(fs);

    s_checkSrcError(fs);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    s_checkProgError(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void s_checkSrcError(GLuint shader) {
    int code = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &code);
    if (code == 0) {
        char* logBuf = malloc(1024 * sizeof(char));
        glGetShaderInfoLog(shader, 1024, NULL, logBuf);
        printf("%s\n", logBuf);
    }
}

void s_checkProgError(GLuint program) {
    int code = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &code);
    if (code == 0) {
        char* logBuf = malloc(1024 * sizeof(char));
        glGetProgramInfoLog(program, 1024, NULL, logBuf);
        printf("%s\n", logBuf);
    }
}

void t_init(Tessellator* tes) {
    // TODO: actually implement this shi
    tes->vao = 0;
    tes->vbo = 0;
    tes->mode = 0;
    tes->verticies = (Vertex*) malloc(tes->capacity * sizeof(Vertex));
    tes->capacity = 1024;
    tes->vertexCount = 0;
    tes->shaderProgram = 0;
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
