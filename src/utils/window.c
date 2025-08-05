#include <stdio.h>
#include <stdlib.h>
#include <glad.h>
#include "state.h"
#include "utils/sound.h"
#include "alhelpers.h"
#include "networking/steam_helpers.h"

#include "gfx/font.h"
#include "gfx/postprocess.h"
#include "gfx/sprite_renderer.h"

#include "utils/debug_shapes.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/console.h>
#endif

extern PostProcessor postProcessor;

// Global function pointers for main loop
static void (*update_func)() = NULL;
static void (*ui_update_func)() = NULL;
static float lastFrame = 0.0f;

#ifdef __EMSCRIPTEN__
void main_loop() {
    float currentFrame = (float)glfwGetTime();
    state.deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;  
    state.time += state.deltaTime;
    
    beginPostProcessing(&postProcessor);
    glClear(GL_COLOR_BUFFER_BIT);

    if (update_func) update_func();
    if (ui_update_func) ui_update_func();

    endPostProcessing(&postProcessor);
    renderPostProcessed(&postProcessor, state.time);

    glfwSwapBuffers(state.window);
    glfwPollEvents();
}
#endif

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

float getMouseAngle() {
    HMM_Vec2 projDims = getProjectionDimensions();
    HMM_Vec2 mouseWorld = mouseToWorld(state.mouseX, state.mouseY);
    HMM_Vec2 direction = HMM_NormV2((HMM_Vec2){projDims.X/2.0f - mouseWorld.X, projDims.Y/2.0f - mouseWorld.Y});
    return atan2f(direction.Y, direction.X);
}

void window_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;

    state.windowWidth = width;
    state.windowHeight = height;
    glViewport(0, 0, width, height);
    resizePostProcessor(&postProcessor, width, height);
}

void InitializeWindow(void (*start)(), void (*update)(), void (*input)(GLFWwindow* window, int key, int scancode, int action, int mods), void (*ui_update)())
{
    //Init GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    
#ifdef __EMSCRIPTEN__
    // For Emscripten/WebGL 2.0, use these specific settings
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // Don't set profile for Emscripten, it will use WebGL automatically
#else
    // For native builds, use OpenGL 4.5
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    state.window = glfwCreateWindow( 1024, 768, "Game", NULL, NULL);
    if( state.window == NULL ){
        fprintf( stderr, "GLFW not happy" );
        glfwTerminate();
        return;
    }

    glfwSetInputMode(state.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    state.cursorDisabled = 0;
    glfwMakeContextCurrent(state.window);
    glfwSetCursorPosCallback(state.window, mouse_callback);
    glfwSetWindowSizeCallback(state.window, window_size_callback);
    glfwSetKeyCallback(state.window, input);

    #ifdef __EMSCRIPTEN__
    if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf( stderr, "GLAD not happy" );
        return;
    }
    #else
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf( stderr, "GLAD not happy" );
        return;
    }
    #endif

#ifndef __EMSCRIPTEN__
    if(InitAL() != 0)
    {
        fprintf(stderr, "Failed to initialize OpenAL\n");
        return;
    }
#endif

    //initSteamAPI();

    const int initialWindowHeight = 768;
    const int initialWindowWidth = 1024;

    glViewport(0, 0, initialWindowWidth, initialWindowHeight);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(1.0f, 0.1f, 0.1f, 0.0f);

    state.windowHeight = initialWindowHeight;
    state.windowWidth = initialWindowWidth;
    state.deltaTime = 0.0f;
    state.time = 0;
    state.tileDim = state.windowWidth / 12;
    state.camX = 0.0f;
    state.camY = 0.0f;
    state.referenceWidth = initialWindowWidth;
    state.referenceHeight = initialWindowHeight;

    start();

    lastFrame = 0.0f;

    glDisable(GL_DEPTH_TEST);

#ifdef __EMSCRIPTEN__
    // Store function pointers for main loop
    update_func = update;
    ui_update_func = ui_update;
    
    // Use Emscripten's main loop
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    // Standard main loop for native builds
    while(!glfwWindowShouldClose(state.window))
    {      
        float currentFrame = (float)glfwGetTime();
        state.deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  
        state.time += state.deltaTime;
        beginPostProcessing(&postProcessor);
        glClear(GL_COLOR_BUFFER_BIT);

        update();
        ui_update();

        endPostProcessing(&postProcessor);
        renderPostProcessed(&postProcessor, state.time);

        glfwSwapBuffers(state.window);
        glfwPollEvents();
    }
#endif
}

// Prolly won't use
void Cleanup()
{
    glfwTerminate();
    CloseAL();
}