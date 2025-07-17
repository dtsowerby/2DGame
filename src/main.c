#include <stdio.h>
#include <math.h>

#include <glad.h>
#include <glfw3.h>
#include <HandmadeMath.h>

#include "utils/types.h"
#include "utils/window.h"
#include "utils/sound.h"
#include "utils/files.h"
#include "utils/animation.h"

#include "gfx/shader.h"
#include "gfx/texture.h"
#include "gfx/sprite_renderer.h"

#include "entity.h"

#include "state.h"

void move_update();

State state;

Entity player;
Entity enemy;
Entity handBomb;

Entity bombShadow;
HMM_Vec2 normDir;
HMM_Vec2 normDirHandBomb;
float initialTime;
float timeAfterBomb = 0;

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

// Player animations
Animation playerIdleDown;
Animation playerIdleUp;

/*
 * Animation System Usage Example:
 * 
 * 1. Create animations with frame arrays:
 *    unsigned int walkFrames[] = {1, 2, 3, 4};
 *    Animation walkAnim = createAnimation(walkFrames, 4, 8.0f, ANIMATION_LOOP);
 * 
 * 2. For simple 2-frame toggle animations (like the old pt system):
 *    Animation toggleAnim = createToggleAnimation(frame1, frame2, 5.0f);
 * 
 * 3. Set animation on entity:
 *    setEntityAnimation(&player, walkAnim);
 * 
 * 4. Update animations each frame:
 *    updateEntityAnimation(&player, state.deltaTime);
 * 
 * 5. The entity will automatically use the current animation frame when drawn
 * 
 * Animation Types:
 * - ANIMATION_LOOP: Repeats forever (good for idle, walk cycles)
 * - ANIMATION_ONCE: Plays once then stops (good for attacks, jumps)
 * - ANIMATION_PINGPONG: Plays forward then backward (good for breathing effects)
 */

void start()
{   
    initSpriteRenderer();

    tileShaderProgram = createShaderProgramS("res/shaders/sprite.vert", "res/shaders/tile.frag");
    tilemap.texture = loadTexture("res/art/tiles1.0.png");
    tilemap.tileWidth = 16;
    tilemap.tileCountX = 8;

    // Create player animations
    unsigned int idleDownFrames[] = {13, 14};
    unsigned int idleUpFrames[] = {15, 21};
    
    playerIdleDown = createAnimation(idleDownFrames, 2, 5.0f, ANIMATION_LOOP);
    playerIdleUp = createAnimation(idleUpFrames, 2, 5.0f, ANIMATION_LOOP);

    player.tileID = 14;
    player.shaderProgram = tileShaderProgram;
    player.tilemap = &tilemap;
    player.position = (HMM_Vec2){0.5f, 0.5f};
    player.scale = (HMM_Vec2){200.0f, 200.0f};
    player.colour = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    player.rotation = 0.0f;
    player.isFlipped = 0;
    player.hasAnimation = 0; // Start without animation, will be set based on state

    /*enemy.shaderProgram = createShaderProgramS("res/shaders/sprite.vert", "res/shaders/sprite.frag");
    enemy.texture = loadTexture("res/dungeonart/2D Pixel Dungeon Asset Pack/Character_animation/monsters_idle/skeleton1/v1/skeleton_v1_4.png");
    enemy.position = (HMM_Vec2){0.2f, 0.2f};
    enemy.scale = (HMM_Vec2){200.0f, 200.0f};
    enemy.colour = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    enemy.rotation = 0.0f;*/

    handBomb.tileID = 32;
    handBomb.shaderProgram = tileShaderProgram;
    handBomb.tilemap = &tilemap;
    handBomb.scale = (HMM_Vec2){200.0f, 200.0f};
    handBomb.colour = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    handBomb.rotation = 0.0f;
    handBomb.isFlipped = 0;
    handBomb.isVisible = 0;
    handBomb.hasAnimation = 0;

    bombShadow.tileID = 40;
    bombShadow.shaderProgram = tileShaderProgram;
    bombShadow.tilemap = &tilemap;
    bombShadow.scale = (HMM_Vec2){200.0f, 200.0f};
    bombShadow.colour = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    bombShadow.rotation = 0.0f;
    bombShadow.isFlipped = 0;
    bombShadow.isVisible = 0;
    bombShadow.hasAnimation = 0;

    explode = loadSound("res/sounds/boom_x.wav");
    bugle = loadSound("res/sounds/call_to_arms.wav");

    //map = parseMapFile("res/maps/test.map");
    map.height = 16;
    map.width = 13;
}

int pt = 0;
void game_update()
{   
    move_update();

    //Generate map
    for(unsigned int i = 0; i < map.height; i++)
    {
        for(unsigned int j = 0; j < map.width; j++)
        {           
            drawTile(&tilemap, mapData[i][j], 
                (HMM_Vec2){((float)j/state.windowWidth)*state.tileDim, 
                ((float)i/state.windowHeight)*state.tileDim - 0.6f}, 
                (HMM_Vec3){1.0f, 1.0f, 1.0f}, 0, tileShaderProgram
            );
        }
    }

    // Handle player flipping based on mouse position
    if(state.mouseX < state.windowWidth/2) {
        player.isFlipped = 1;
    } else {
        player.isFlipped = 0;
    }
    
    // Handle player animation based on mouse Y position
    if(state.windowHeight/2.0f - state.windowHeight/10.0f < state.mouseY) {
        // Mouse is in lower area - use idle down animation
        if (!player.hasAnimation || player.animation.frames != playerIdleDown.frames) {
            setEntityAnimation(&player, playerIdleDown);
        }
    } else {
        // Mouse is in upper area - use idle up animation  
        if (!player.hasAnimation || player.animation.frames != playerIdleUp.frames) {
            setEntityAnimation(&player, playerIdleUp);
        }
    }
    
    // Update all entity animations
    updateEntityAnimation(&player, state.deltaTime);
    updateEntityAnimation(&handBomb, state.deltaTime);
    updateEntityAnimation(&bombShadow, state.deltaTime);

    if(bombShadow.isVisible) {
        bombShadow.position.X -= normDir.X * state.deltaTime;
        bombShadow.position.Y -= normDir.Y * state.deltaTime;
        drawEntity(&bombShadow);
    }
    drawEntity(&player);
    
    timeAfterBomb = (float)(state.time - initialTime);
    if(!bombShadow.isVisible) {
        handBomb.position.X = player.position.X - normDirHandBomb.X * 0.03f;
        handBomb.position.Y = player.position.Y - normDirHandBomb.Y * 0.03f - abs(sin(timeAfterBomb*3.0f)) * 0.1f;
        drawEntity(&bombShadow);
    } else {
        handBomb.position.X -= normDir.X * state.deltaTime;
        handBomb.position.Y -= normDir.Y * state.deltaTime;
        drawEntity(&bombShadow);
    }
    drawEntity(&handBomb);
}

void camera_update(double mousePosX, double mousePosY)
{   
    HMM_Vec2 playerWorld = (HMM_Vec2){((player.position.X - 0.5f)*state.windowWidth), ((player.position.Y - 0.5f)*state.windowHeight)};
    state.camX = playerWorld.X + (mousePosX - (state.windowWidth/2.0f))/4.0f;
    state.camY = playerWorld.Y + (mousePosY - (state.windowHeight/2.0f))/4.0f;
}

void move_update()
{
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
}

void input(GLFWwindow* window, int key, int scancode, int action, int mods)
{   
    //playSound(bugle);
    //playSound(explode);
    if(action == GLFW_PRESS){
        switch(key)
        {
            case GLFW_KEY_ESCAPE:
                glfwTerminate();
                exit(0);
                break;
            case GLFW_KEY_W:
                player.position.Y -= 0.01f;
                camera_update(state.mouseX, state.mouseY);
                break;
            case GLFW_KEY_S:
                player.position.Y += 0.01f;
                camera_update(state.mouseX, state.mouseY);
                break;
            case GLFW_KEY_A:
                player.position.X -= 0.01f;
                camera_update(state.mouseX, state.mouseY);
                break;
            case GLFW_KEY_D:
                player.position.X += 0.01f;
                camera_update(state.mouseX, state.mouseY);
                break;
            case GLFW_KEY_SPACE:
                if(bombShadow.isVisible == 0) {
                    bombShadow.position = (HMM_Vec2){player.position.X, player.position.Y};
                    bombShadow.isVisible = 1;
                    normDir = HMM_NormV2((HMM_Vec2){state.windowWidth/2 - state.mouseX, state.windowHeight/2 - state.mouseY});
                    initialTime = state.time;
                } else {   
                    normDirHandBomb = HMM_NormV2((HMM_Vec2){(float)state.windowWidth/2.0f - state.mouseX, (float)state.windowHeight/2.0f - state.mouseY});
                    bombShadow.isVisible = 0;
                }
                break;
        }
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

int main(int argc, char** argv)
{   
    InitializeWindow(start, game_update, input, ui_update);
    return 0;
}
