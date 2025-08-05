#pragma once

#include <GLFW/glfw3.h>

typedef enum GameState {
    STATE_GAMEPLAY,
    STATE_CUSTOMIZE,
    STATE_MENU,
    STATE_SETTINGS,
    STATE_TUTORIAL,
    STATE_DEATH
} GameState;

typedef struct State
{
    GLFWwindow* window;
    int windowWidth;
    int windowHeight;
    char cursorDisabled;
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
    unsigned int stage;
    int stageCleared;

    unsigned int highScore;
    int tutorial;
} State;

extern struct State state;
