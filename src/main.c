#include <stdio.h>

#include <glad.h>
#include <glfw3.h>
#include <HandmadeMath.h>

#include "utils/window.h"
#include "utils/sound.h"

#include "gfx/shader.h"
#include "gfx/texture.h"
#include "gfx/sprite_renderer.h"

#include "entity.h"

#include "state.h"

State state;

Entity player;
Entity enemy;
Entity item;

ALuint explode;
ALuint bugle;

// Map dimensions: 12x12
// Original dimensions: 12x12
// Tile dimensions: 16x16
int map[12][12] = {
    {78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78},
    {78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78},
    {78, 78, 78, 78, 78, 40, 10, 10, 10, 10, 0, 78},
    {78, 78, 78, 78, 78, 41, 31, 21, 21, 11, 1, 78},
    {78, 78, 78, 78, 78, 41, 32, 9, 22, 12, 1, 78},
    {78, 78, 78, 78, 78, 41, 32, 9, 9, 12, 1, 78},
    {78, 78, 78, 78, 78, 42, 32, 22, 22, 12, 1, 78},
    {78, 78, 78, 78, 78, 43, 32, 9, 9, 12, 1, 78},
    {78, 78, 78, 78, 78, 44, 34, 24, 24, 14, 1, 78},
    {78, 78, 78, 78, 78, 45, 25, 25, 25, 25, 5, 78},
    {78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78},
    {78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78}
};



void start()
{   
    initSpriteRenderer();

    player.shaderProgram = createShaderProgramS("res/shaders/sprite.vert", "res/shaders/sprite.frag");
    player.texture = loadTexture("res/awesomeface.png");
    player.position = (HMM_Vec2){0.5f, 0.5f};
    player.scale = (HMM_Vec2){200.0f, 200.0f};
    player.colour = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    player.rotation = 0.0f;

    enemy.shaderProgram = createShaderProgramS("res/shaders/sprite.vert", "res/shaders/sprite.frag");
    enemy.texture = loadTexture("res/dungeonart/2D Pixel Dungeon Asset Pack/Character_animation/monsters_idle/skeleton1/v1/skeleton_v1_4.png");
    enemy.position = (HMM_Vec2){0.2f, 0.2f};
    enemy.scale = (HMM_Vec2){200.0f, 200.0f};
    enemy.colour = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    enemy.rotation = 0.0f;

    item.shaderProgram = createShaderProgramS("res/shaders/sprite.vert", "res/shaders/tile.frag");
    item.texture = loadTexture("res/dungeonart/2D Pixel Dungeon Asset Pack/character and tileset/Dungeon_Tileset.png");
    item.position = (HMM_Vec2){0.8f, 0.2f};
    item.scale = (HMM_Vec2){state.windowWidth / 12.0f, state.windowWidth / 12.0f};
    item.colour = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    item.rotation = 0.0f;

    explode = loadSound("res/sounds/boom_x.wav");
    bugle = loadSound("res/sounds/call_to_arms.wav");
}

void game_update()
{   
    for(int i = 0; i < 12; i++)
    {
        for(int j = 0; j < 12; j++)
        {   
            item.position = (HMM_Vec2){(float)i / 12.0f, 1.0f - (float)j / 12.0f}; // Set position based on tile index
            drawTileEntity(item, map[i][j]); // Draw the first tile from the tileset
        }
    }

    drawEntity(player);
    player.rotation += 0.01f;

    drawEntity(enemy);
}

void input()
{   
    if (glfwGetKey(state.window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwTerminate();
        exit(0);
    }
    if (glfwGetKey(state.window, GLFW_KEY_W) == GLFW_PRESS)
    {   
        player.position.Y -= 0.01f;
    }
    if (glfwGetKey(state.window, GLFW_KEY_S) == GLFW_PRESS)
    {   
        player.position.Y += 0.01f;
    }
    if (glfwGetKey(state.window, GLFW_KEY_A) == GLFW_PRESS)
    {
        player.position.X -= 0.01f;
    }
    if (glfwGetKey(state.window, GLFW_KEY_D) == GLFW_PRESS)
    {
        player.position.X += 0.01f;
    }
    if (glfwGetKey(state.window, GLFW_KEY_E) == GLFW_PRESS)
    {
        playSound(bugle);
    }
    if (glfwGetKey(state.window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        playSound(explode);
    }
}

void ui_update()
{

}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{   

}

int main(void)
{   
    InitializeWindow(start, game_update, input, ui_update);
    return 0;
}