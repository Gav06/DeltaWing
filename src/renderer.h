#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLuint Shader_createProgram(const char* vertexShader, const char* fragShader);

const char* Shader_loadShaderSource(const char* path);