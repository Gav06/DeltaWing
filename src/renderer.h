
typedef struct {
    int program;
    
} Shader;

void Shader_createProgram(char* vertexShader, char* fragShader, int* program);

char* Shader_loadShaderSource(char* path);