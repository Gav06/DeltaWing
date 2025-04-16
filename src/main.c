#include <stdlib.h>
#include <stdio.h>

#define DEFINE_GLOBALS
#include "globals.h"

#include "util.h"
#include "scenes.h"

GLFWwindow *window;

uint32_t fps;
uint32_t tps;

int32_t cameraX = 0;
int32_t cameraY = 0;

bool running = true;

// Our scene defaults to the main menu

void DW_GLFWerrorCallback(int error, const char *description) {
    fprintf(stderr, "Error: %d %s\n", error, description);
}

void DW_GLerrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
    fprintf(stderr, "GL ERROR: %s\n", message);
}

void DW_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key >= 32 && key <= 348) {
        input->keyStates[key] = action; 
        input->currentMods = mods;

        if (currentScene != NULL) currentScene->onKey(key, scancode, action, mods);
    }
}

void DW_mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    if (0 <= button) {
        input->mouseState[button] = action;

        if (currentScene != NULL) currentScene->onClick(button, action, mods);
    }
}

void DW_cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
    input->prevMouseX = input->mouseX;
    input->prevMouseY = input->mouseY;
    input->mouseX = (uint32_t) xpos;
    input->mouseY = (uint32_t) ypos;
}

bool DW_initWindow() {    
    if (!glfwInit()) {
        return true;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);  

    window = glfwCreateWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "DeltaWing", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // setup callbacks
    glfwSetErrorCallback(DW_GLFWerrorCallback);
    glfwSetKeyCallback(window, DW_keyCallback);
    glfwSetMouseButtonCallback(window, DW_mouseButtonCallback);
    glfwSetCursorPosCallback(window, DW_cursorPosCallback);

    if (!window) {
        fprintf(stderr, "Error: Unable to create GLFW window");
        glfwTerminate();
        return true;
    }

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "Error: Couldn't load OpenGL\n");
        return true;
    }

    const unsigned char *version = glGetString(GL_VERSION);
    const unsigned char *renderer = glGetString(GL_RENDERER);
    printf("OpenGL: %s\n", version);
    printf("Renderer: %s\n", renderer);

    glEnable(GL_DEBUG_OUTPUT);
    // Disable all messages for the INFO and DEBUG severity levels
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_FALSE);
    glDebugMessageCallback((GLDEBUGPROC) DW_GLerrorCallback, 0);

    // setup our GL state a little bit
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return false;
}

void DW_setScene(Scene_t *scene) {
    if (scene != NULL) {

        if (currentScene != NULL) currentScene->exit();

        currentScene = scene;
        scene->init();
    }
}

const float left = (DISPLAY_WIDTHF / 2.0f) - 256.0f;
const float right = (DISPLAY_WIDTHF / 2.0f) + 256.0f;
const float top = (DISPLAY_HEIGHTF / 2.0f) - 256.0f;
const float bottom = (DISPLAY_HEIGHTF / 2.0f) + 256.0f;

GLuint testTexture;

Renderer_t *testRenderer;

void DW_initGame() {
    // Compile shaders for all of our vertex formats
    Shader_compileDefaultShaders();

    // init render context
    context = (Context_t*) malloc(sizeof(Context_t));
    Context_init(context, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    // create font renderer
    fontRenderer = (FontRenderer_t*) malloc(sizeof(FontRenderer_t));
    FontRenderer_init(fontRenderer, context, "assets/roboto_mono.fnt", 0.5f);

    // Create keyboard input struct, with zeroes (false as default key states)
    input = (Input_t*) calloc(1, sizeof(Input_t));

    // Init default scene
    currentScene = &Scene_MainMenu;
    if (currentScene != NULL) currentScene->init();

    testRenderer = (Renderer_t*) malloc(sizeof(Renderer_t));

    Vertex_PT verticies[] = {
        { { left, bottom, 0.0f }, { 0.0f, 0.0f } },
        { { left, top, 0.0f }, { 0.0f, 1.0f } },
        { { right, top, 0.0f }, { 1.0f, 1.0f } },
        { { right, bottom, 0.0f }, { 1.0f, 0.0f } }
    };

    uint32_t indicies[] = {
        0, 1, 2, 0, 2, 3
    };

    VertexBuffer_t vb;
    VertexBuffer_init(&vb, sizeof(Vertex_PT), sizeof(verticies) / sizeof(Vertex_PT), sizeof(verticies), GL_STATIC_DRAW, verticies);
    IndexBuffer_t ib;
    IndexBuffer_init(&ib, 6, sizeof(indicies), indicies);

    Renderer_init(testRenderer, context, VERTEX_FORMAT_PT, &vb, &ib);
}

void DW_exitGame() {
    running = false;
}

void DW_cleanup() {
    if (!glfwWindowShouldClose(window)) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    // Free up vram and heap
    FontRenderer_free(fontRenderer);
    Context_free(context);

    free(input);
    input = NULL;
}

void DW_tick() {
    if (currentScene != NULL) {
        currentScene->tick();
    }
}

void DW_render(float partialTicks) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw current scene
    if (currentScene != NULL) {
        currentScene->render();
    }

}

int main(int argc, char **argv) {

    if (DW_initWindow()) return 1;
    
    DW_initGame();

    glfwShowWindow(window);

    uint64_t lastTime = DW_currentTimeMillis();
    uint64_t accumulator = 0;
    uint64_t lastFPSTime = lastTime;
    uint32_t ticks = 0;
    uint32_t frames = 0;

    glClearColor(.1f, .1f, .1f, 1.0f);
    // Game loop
    while (running) {
        uint64_t currentTime = DW_currentTimeMillis();
        uint64_t deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        // Cap deltaTime to prevent spiral of death if game hangs
        if (deltaTime > MAX_DELTA_TIME)
            deltaTime = MAX_DELTA_TIME;
            
        accumulator += deltaTime;
        
        // Process physics at fixed time step
        while (accumulator >= MS_PER_TICK) {
            DW_tick();
            ticks++;
            accumulator -= MS_PER_TICK;
        }
        
        // Calculate partial ticks for smooth rendering
        const float partialTicks = (float)accumulator / MS_PER_TICK;
        context->partialTicks = partialTicks;
        
        // Render
        DW_render(partialTicks);

        frames++;
        
        // FPS counter
        if (currentTime - lastFPSTime >= 1000) {
            fps = frames;
            tps = ticks;

            frames = 0;
            ticks = 0;

            lastFPSTime = currentTime;
            printf("FPS %d TPS %d\n", fps, tps);

        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwWindowShouldClose(window)) running = false;
    }

    // This must be called before destroying our context because
    // We need some GL functions to free our VRAM
    DW_cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
