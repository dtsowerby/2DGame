#pragma once

#include <glfw3.h>

typedef enum GameState {
    STATE_GAMEPLAY,
    STATE_CUSTOMIZE,
    STATE_MENU,
    STATE_SETTINGS
} GameState;

typedef struct State
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
    unsigned int tileDim;

    double camX;
    double camY;

    double mouseX;
    double mouseY;

    int referenceWidth;
    int referenceHeight;

    GameState gameState;
} State;

extern struct State state;
