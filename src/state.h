#pragma once

#include <glfw3.h>

struct State
{
    GLFWwindow* window;
    int windowWidth;
    int windowHeight;
    char cursorDisabled;
    union clearColor
    {
        int i[3];
        struct {int x; int y; int z;};
    };
    float deltaTime;
    float time;
    unsigned int spriteVAO;
    unsigned int spriteVBO;
};

struct State state;
