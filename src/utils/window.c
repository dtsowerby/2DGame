#include <stdio.h>
#include <stdlib.h>
#include <glad.h>
#include "state.h"
#include "utils/sound.h"
#include "ALhelpers.h"
#include "networking/steam_helpers.h"

#include "gfx/font.h"
#include "gfx/postprocess.h"
#include "utils/debug_shapes.h"

extern PostProcessor postProcessor;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf( stderr, "GLAD not happy" );
        return;
    }

    if(InitAL() != 0)
    {
        fprintf(stderr, "Failed to initialize OpenAL\n");
        return;
    }

    //initSteamAPI();

    const int initialWindowHeight = 768;
    const int initialWindowWidth = 1024;

    glViewport(0, 0, initialWindowWidth, initialWindowHeight);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

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

    float lastFrame = 0.0f;

    glDisable(GL_DEPTH_TEST);

    while(!glfwWindowShouldClose(state.window))
    {      
        float currentFrame = (float)glfwGetTime();
        state.deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  
        state.time += state.deltaTime;
        beginPostProcessing(&postProcessor);
        glClear(GL_COLOR_BUFFER_BIT);

        ui_update();
        update();

        flushDebugShapes();
        flushFontBatch();

        endPostProcessing(&postProcessor);
        renderPostProcessed(&postProcessor, state.time);

        glfwSwapBuffers(state.window);
        glfwPollEvents();
    }
}

// Prolly won't use
void Cleanup()
{
    glfwTerminate();
    CloseAL();
}