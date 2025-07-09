#include <stdio.h>
#include <math.h>

#include <glad.h>
#include <glfw3.h>
#include <HandmadeMath.h>

#include "utils/types.h"
#include "utils/window.h"
#include "utils/sound.h"
#include "utils/files.h"

#include "gfx/shader.h"
#include "gfx/texture.h"
#include "gfx/sprite_renderer.h"

#include "entity.h"

#include "state.h"

State state;

Entity player;
Entity enemy;

Sound explode;
Sound bugle;

unsigned int tileShaderProgram;

Map map;
unsigned int mapData[16][15] = {
    {0,1,2,3,1,2,1,2,1,3,1,3,4},
    {16,19,11,18,10,11,19,11,18,17,10,17,20},
    {8,10,11,9,10,11,10,10,17,11,9,10,12},
    {16,19,11,18,10,11,19,17,18,10,11,17,20},
    {8,17,17,17,18,17,11,17,11,11,9,11,20},
    {16,19,11,18,17,11,19,11,18,10,11,17,20},
    {8,19,11,18,10,17,9,18,10,17,10,11,12},
    {16,19,11,18,11,17,11,18,10,11,17,10,20},
    {8,10,11,17,10,17,9,11,17,10,11,9,12},
    {16,19,11,18,11,17,11,18,10,11,17,10,20},
    {8,10,11,17,10,17,9,11,17,10,11,9,12},
    {16,19,11,18,11,11,17,18,10,11,17,10,20},
    {8,10,11,17,10,9,11,17,10,11,17,9,12},
    {16,19,11,18,11,11,18,17,10,11,17,10,20},
    {8,10,11,17,10,17,9,11,17,10,11,9,12},
    {24,25,27,26,27,25,27,26,27,25,26,27,28}
};

// Map dimensions: 8x8
// Tile dimensions: 16x16
Tilemap tilemap;

void start()
{   
    initSpriteRenderer();

    player.shaderProgram = createShaderProgramS("res/shaders/sprite.vert", "res/shaders/sprite.frag");
    player.texture = loadTexture("res/awesomeface.png");
    player.position = (HMM_Vec2){0.5f, 0.5f};
    player.scale = (HMM_Vec2){200.0f, 200.0f};
    player.colour = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    player.rotation = 0.0f;

    /*enemy.shaderProgram = createShaderProgramS("res/shaders/sprite.vert", "res/shaders/sprite.frag");
    enemy.texture = loadTexture("res/dungeonart/2D Pixel Dungeon Asset Pack/Character_animation/monsters_idle/skeleton1/v1/skeleton_v1_4.png");
    enemy.position = (HMM_Vec2){0.2f, 0.2f};
    enemy.scale = (HMM_Vec2){200.0f, 200.0f};
    enemy.colour = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    enemy.rotation = 0.0f;*/

    tileShaderProgram = createShaderProgramS("res/shaders/sprite.vert", "res/shaders/tile.frag");
    tilemap.texture = loadTexture("res/art/tles1.0.png");
    tilemap.tileWidth = 16;
    tilemap.tileCountX = 8;

    explode = loadSound("res/sounds/boom_x.wav");
    bugle = loadSound("res/sounds/call_to_arms.wav");

    //map = parseMapFile("res/maps/test.map");
    map.height = 16;
    map.width = 13;
}

int pt = 0;
void game_update()
{   
    for(unsigned int i = 0; i < map.height; i++)
    {
        for(unsigned int j = 0; j < map.width; j++)
        {           
            //printf("%u ", mapData[j][i]);
            drawTile(&tilemap, mapData[i][j], 
                (HMM_Vec2){((float)j/state.windowWidth)*state.tileDim, 
                ((float)i/state.windowHeight)*state.tileDim - 0.6f}, 
                (HMM_Vec3){1.0f, 1.0f, 1.0f}, tileShaderProgram
            ); // Draw the first tile from the tilesetddddddddd
        }
        //printf("\n");
    }

    //drawEntity(&player);
    //Crappy Animation
    pt = (int)round(state.time*5) & 1;
    if(pt) {
        drawTile(&tilemap, 5, player.position, player.colour, tileShaderProgram);
    } else {
        drawTile(&tilemap, 6, player.position, player.colour, tileShaderProgram);
    }
    //player.rotation += 0.01f;

    //drawEntity(enemy);
}

void camera_update(double mousePosX, double mousePosY)
{   
    HMM_Vec2 playerWorld = (HMM_Vec2){((player.position.X - 0.5f)*state.windowWidth), ((player.position.Y - 0.5f)*state.windowHeight)};
    state.camX = playerWorld.X + (mousePosX - (state.windowWidth/2.0f))/4.0f;
    state.camY = playerWorld.Y + (mousePosY - (state.windowHeight/2.0f))/4.0f;
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
        camera_update(state.mouseX, state.mouseY);
    }
    if (glfwGetKey(state.window, GLFW_KEY_S) == GLFW_PRESS)
    {   
        player.position.Y += 0.01f;
        camera_update(state.mouseX, state.mouseY);
    }
    if (glfwGetKey(state.window, GLFW_KEY_A) == GLFW_PRESS)
    {
        player.position.X -= 0.01f;
        camera_update(state.mouseX, state.mouseY);
    }
    if (glfwGetKey(state.window, GLFW_KEY_D) == GLFW_PRESS)
    {
        player.position.X += 0.01f;
        camera_update(state.mouseX, state.mouseY);
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
    state.mouseX = xposIn;
    state.mouseY = yposIn;
    camera_update(xposIn, yposIn);
}

int main(void)
{   
    InitializeWindow(start, game_update, input, ui_update);
    return 0;
}