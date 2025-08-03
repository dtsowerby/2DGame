#pragma once

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

float getMouseAngle();

void window_size_callback(GLFWwindow* window, int width, int height);

void InitializeWindow(void (*start)(), void (*update)(), void (*input)(GLFWwindow* window, int key, int scancode, int action, int mods), void (*ui_update)());

// Prolly won't use
void Cleanup();

void toggleFullscreen(GLFWwindow* window) {
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
}
