#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

float getMouseAngle();

void window_size_callback(GLFWwindow* window, int width, int height);

void InitializeWindow(void (*start)(), void (*update)(), void (*input)(GLFWwindow* window, int key, int scancode, int action, int mods), void (*ui_update)());

// Prolly won't use
void Cleanup();

void toggleFullscreen(GLFWwindow* window) {
#ifdef __EMSCRIPTEN__
    // For Emscripten, use HTML5 Fullscreen API
    EmscriptenFullscreenChangeEvent fullscreenStatus;
    if (emscripten_get_fullscreen_status(&fullscreenStatus) == EMSCRIPTEN_RESULT_SUCCESS) {
        if (fullscreenStatus.isFullscreen) {
            // Exit fullscreen
            emscripten_exit_fullscreen();
        } else {
            // Enter fullscreen on the canvas element
            EmscriptenFullscreenStrategy strategy;
            strategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_ASPECT;
            strategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_NONE;
            strategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
            strategy.canvasResizedCallback = 0;
            strategy.canvasResizedCallbackUserData = 0;
            
            emscripten_request_fullscreen_strategy("#canvas", 1, &strategy);
        }
    }
#else
    // Native fullscreen implementation
    if (glfwGetWindowMonitor(window)) {
        int xpos, ypos, width, height;
        xpos = 100;
        ypos = 100;
        width = 800;
        height = 600;
        glfwSetWindowMonitor(window, NULL, xpos, ypos, width, height, 0);
    } else {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
#endif
}
