#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <glad.h>
#include "state.h"
#include "utils/sound.h"
#include "networking/steam_helpers.h"

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

void window_size_callback(GLFWwindow* window, int width, int height) {
    state.windowWidth = width;
    state.windowHeight = height;
    glViewport(0, 0, width, height);
}

void InitializeWindow(void (*start)(), void (*update)(), void (*input)(), void (*ui_update)())
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

    glViewport(0, 0, 1024, 768);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.8f, 0.2f, 0.4f, 0.0f);

    state.windowHeight = 768;
    state.windowWidth = 1024;
    state.deltaTime = 0.0f;
    state.time = 0;
    state.tileDim = state.windowWidth / 12;
    state.camX = 0.0f;
    state.camY = 0.0f;

    start();

    float lastFrame = 0.0f;

    while(!glfwWindowShouldClose(state.window))
    {      
        float currentFrame = (float)glfwGetTime();
        state.deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  
        state.time += state.deltaTime;
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        input();

        ui_update();
        update();

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