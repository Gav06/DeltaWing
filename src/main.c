#include <stdlib.h>
#include <stdio.h>

#include "renderer.h"
#include "engine.h"
#include "input.h"
#include "scenes.h"
#include "font.h"
#include "util.h"

#define DISPLAY_WIDTH 1280
#define DISPLAY_HEIGHT 720

#define DISPLAY_WIDTHF 1280.0f
#define DISPLAY_HEIGHTF 720.0f

#define TARGET_FPS 60
#define MS_PER_FRAME (1000 / TARGET_FPS)

#define TARGET_TPS 30
#define MS_PER_TICK (1000 / TARGET_TPS)

GLFWwindow *window;
Input_t *input;

Context_t *context;
Renderer_t *dynRenderer;
FontRenderer_t *fontRenderer;

uint32_t fps;

int32_t cameraX = 0;
int32_t cameraY = 0;


// Our scene defaults to the main menu
Scene_t *currentScene = &Scene_MainMenu;


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
    }
}

void DW_mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    if (0 <= button <= 8) {
        input->mouseState[button] = action;
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

    const char *version = glGetString(GL_VERSION);
    const char *renderer = glGetString(GL_RENDERER);
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

Renderer_t *testRenderer;

const float left = (DISPLAY_WIDTH / 2) - 256.0f;
const float right = (DISPLAY_WIDTH / 2) + 256.0f;
const float top = (DISPLAY_HEIGHT / 2) - 256.0f;
const float bottom = (DISPLAY_HEIGHT / 2) + 256.0f;

GLuint testTexture;

void DW_initGame() {
    // Compile shaders for all of our vertex formats
    Shader_compileDefaultShaders();

    // init render context
    context = malloc(sizeof(Context_t));
    Context_init(context, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    // create font renderer
    fontRenderer = malloc(sizeof(FontRenderer_t));
    FontRenderer_init(fontRenderer, context, "assets/roboto_mono.fnt");

    // Create keyboard input struct, with zeroes (false as default key states)
    input = calloc(1, sizeof(Input_t));

    // Init default scene
    if (currentScene != NULL) currentScene->init();

}

void DW_exitGame() {
    // Free up vram and heap
    FontRenderer_free(fontRenderer);
    // Renderer_free(dynRenderer);
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
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // draw current scene
    if (currentScene != NULL) {
        currentScene->render(dynRenderer, context);
    }


    // Renderer_bind(fontRenderer->renderer);
    FontRenderer_drawChar(fontRenderer, '\n');
}

int main(int argc, char **argv) {
    if (DW_initWindow())
        return 1;
    
    DW_initGame();

    glfwShowWindow(window);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    uint64_t lastTime = DW_currentTimeMillis();
    uint64_t accumulator = 0;
    uint64_t frameStart, deltaTime;
    uint32_t ticks = 0;

    // Game loop
    while (!glfwWindowShouldClose(window)) {

        // uint64_t currentFrameTime = DW_currentTimeMillis() - startTime;
        frameStart = DW_currentTimeMillis();
        deltaTime = frameStart - lastTime;
        lastTime = frameStart;
        accumulator += deltaTime;

        while (accumulator >= MS_PER_TICK) {
            DW_tick();
            
            ticks++;
            accumulator -= MS_PER_TICK;
        }

        const float partialTicks = (float) accumulator / MS_PER_TICK;
        // partialTicks is apart of Context struct and passed into render so it is accessible from virtually anywhere
        context->partialTicks = partialTicks;
        
        DW_render(partialTicks);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // limit fps
        uint64_t frameEnd = DW_currentTimeMillis();
        uint64_t frameDuration = frameEnd - frameStart;
        if (frameDuration < MS_PER_FRAME) {
            DW_sleepMillis(MS_PER_FRAME - frameDuration);
        }
    }

    // This must be called before destroying our context because
    // We need some GL functions to free our VRAM
    DW_exitGame();

    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
