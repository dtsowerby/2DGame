#pragma once

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

float getMouseAngle();

void window_size_callback(GLFWwindow* window, int width, int height);

void InitializeWindow(void (*start)(), void (*update)(), void (*input)(GLFWwindow* window, int key, int scancode, int action, int mods), void (*ui_update)());

// Prolly won't use
void Cleanup();