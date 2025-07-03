#include <stdio.h>

#include <glad.h>
#include <glfw3.h>
#include <HandmadeMath.h>

#include "utils/window.h"

#include "gfx/shader.h"
#include "gfx/texture.h"
#include "gfx/sprite_renderer.h"

#include "entity.h"

#include "state.h"

Entity player;
Entity enemy;
Entity item;

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
    item.scale = (HMM_Vec2){200.0f, 200.0f};
    item.colour = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    item.rotation = 0.0f;
}

void update()
{   
    drawEntity(player);
    player.rotation += 0.01f;

    drawEntity(enemy);

    drawTileEntity(item, 93); // Draw the first tile from the tileset
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
}

void ui_update()
{

}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{   

}

int main(void)
{   
    InitializeWindow(start, update, input, ui_update);
    return 0;
}