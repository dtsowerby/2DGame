#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <glad.h>
#include <glfw3.h>
#include <HandmadeMath.h>

#include "utils/types.h"
#include "utils/window.h"
#include "utils/sound.h"
#include "utils/files.h"
#include "utils/debug_shapes.h"
#include "utils/random.h"

#include "gfx/shader.h"
#include "gfx/texture.h"
#include "gfx/sprite_renderer.h"
#include "gfx/particle.h"
#include "gfx/primitive.h"

#include "game/collision.h"

#include "entity.h"

#include "state.h"

void move_update();

State state;

Entity player;
Entity bomb;
HMM_Vec2 bombShadowPosition;

HMM_Vec2 normDir;
HMM_Vec2 normDirHandBomb;
float initialTime;
float timeAfterBomb = 0;

Sound explode;
Sound bugle;

unsigned int tileShaderProgram;
unsigned int particleShaderProgram;

// Particle system
ParticleEmitter explosionEmitter;
ParticleEmitter smokeEmitter;

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

Entity entities[100];
char entityExists[100] = {0};

// Map dimensions: 8x8
// Tile dimensions: 16x16
Tilemap tilemap;

float col_one;
float col_two;
float col_three;

void start()
{   
    initSpriteRenderer();
    initDebugShapes();
    initPrimitives();

    tileShaderProgram = createShaderProgramS("res/shaders/sprite.vert", "res/shaders/tile.frag");
    particleShaderProgram = createShaderProgramS("res/shaders/particle.vert", "res/shaders/particle.frag");
    
    tilemap.texture = loadTexture("res/art/tiles1.0.png");
    tilemap.tileWidth = 16;
    tilemap.tileCountX = 15;

    // Initialize particle system
    explosionEmitter = createExplosionEmitter((HMM_Vec2){0.5f, 0.5f}, 50);
    smokeEmitter = createSmokeEmitter((HMM_Vec2){0.3f, 0.3f});
    bomb.particleEmitter = createBombEmitter((HMM_Vec2){0.5f, 0.5f}, 50);

    startEmitter(&smokeEmitter); // Start smoke emitter automatically

    player.position = (HMM_Vec2){500.0f, 500.0f};
    player.scale = (HMM_Vec2){200.0f, 200.0f};
    player.colour = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    player.rotation = 0.0f;
    player.isVisible = 1;

    player.shaderProgram = particleShaderProgram;
    player.particleEmitter = createCharacterEmitter(player.position, 50);
    startEmitter(&player.particleEmitter);

    /*enemy.shaderProgram = createShaderProgramS("res/shaders/sprite.vert", "res/shaders/sprite.frag");
    enemy.texture = loadTexture("res/dungeonart/2D Pixel Dungeon Asset Pack/Character_animation/monsters_idle/skeleton1/v1/skeleton_v1_4.png");
    enemy.position = (HMM_Vec2){0.2f, 0.2f};
    enemy.scale = (HMM_Vec2){200.0f, 200.0f};
    enemy.colour = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    enemy.rotation = 0.0f;*/

    bomb.shaderProgram = tileShaderProgram;
    bomb.scale = (HMM_Vec2){200.0f, 200.0f};
    bomb.colour = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    bomb.rotation = 0.0f;
    bomb.isVisible = 1;

    explode = loadSound("res/sounds/boom_x.wav");
    bugle = loadSound("res/sounds/call_to_arms.wav");

    //map = parseMapFile("res/maps/test.map");
    map.height = 16;
    map.width = 13;

    init_random();

    col_one = random_range(0.3f, 0.7f);
    col_two = random_range(0.3f, 0.7f);
    col_three = random_range(0.3f, 0.7f);
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
                (HMM_Vec2){((float)j)*state.tileDim + sin(state.time * i) * 4.0f, 
                ((float)i)*state.tileDim - 0.6f + cos(state.time * j) * 4.0f}, 
                (HMM_Vec3){fabs((i+j)%2 - col_one),
                           fabs((i+j)%2 - col_two), 
                           fabs((i+j)%2 - col_three)}, 0,  particleShaderProgram 
            );
        }
    }
    
    timeAfterBomb = (float)(state.time - initialTime);
    if(bomb.isVisible && sin(timeAfterBomb*7.2f) < 0.0f)
    {
        explosionEmitter.position = bomb.position;
        emitBurst(&explosionEmitter, 30);
        bomb.isVisible = 0;
        bomb.particleEmitter.isActive = 0;
    }
    if(!bomb.isVisible) {
        HMM_Vec2 projDims = getProjectionDimensions();
        HMM_Vec2 mouseWorld = mouseToWorld(state.mouseX, state.mouseY);
        normDirHandBomb = HMM_NormV2((HMM_Vec2){projDims.X/2.0f - mouseWorld.X, projDims.Y/2.0f - mouseWorld.Y});
        bomb.position.X = player.position.X - normDirHandBomb.X * 50.0f;
        bomb.position.Y = player.position.Y - normDirHandBomb.Y * 50.0f;
    } else {
        bomb.position.X -= normDir.X * state.deltaTime * 400.0f;
        bomb.position.Y -= normDir.Y * state.deltaTime * 400.0f;
        bombShadowPosition.X -= normDir.X * state.deltaTime * 400.0f;
        bombShadowPosition.Y -= normDir.Y * state.deltaTime * 400.0f;

        bomb.position.Y -= (float)sin(timeAfterBomb*15.0f) * 15.0f;
        drawEntity(&bomb);
        drawSquare((HMM_Vec2){bombShadowPosition.X, bombShadowPosition.Y}, 20.0f, 45.0f, (HMM_Vec3){0.0f, 0.0f, 0.0f}, particleShaderProgram);
        drawSquare((HMM_Vec2){bombShadowPosition.X, bombShadowPosition.Y}, 18.0f, 135.0f, (HMM_Vec3){0.0f, 0.0f, 0.0f}, particleShaderProgram);
    }
    
    // Update particle systems
    updateParticleEmitter(&explosionEmitter);
    updateParticleEmitter(&smokeEmitter);

    updateEntity(&player);
    updateEntity(&bomb);

    drawEntity(&player);

    renderParticleEmitter(&explosionEmitter, particleShaderProgram);
    renderParticleEmitter(&smokeEmitter, particleShaderProgram);

    //drawSquare((HMM_Vec2){player.position.X + 150.0f, player.position.Y}, 80.0f, 0.0f, (HMM_Vec3){1.0f, 0.2f, 0.2f}, particleShaderProgram);
    //drawRect((HMM_Vec2){player.position.X, player.position.Y + 120.0f}, (HMM_Vec2){120.0f, 60.0f}, 0.0f, (HMM_Vec3){0.2f, 1.0f, 0.2f}, particleShaderProgram);
    //drawCircle((HMM_Vec2){player.position.X - 150.0f, player.position.Y}, 50.0f, (HMM_Vec3){0.2f, 0.2f, 1.0f}, particleShaderProgram);
    //drawSquare((HMM_Vec2){player.position.X, player.position.Y - 120.0f}, 60.0f, state.time * 2.0f, (HMM_Vec3){1.0f, 1.0f, 0.2f}, particleShaderProgram);

    /*drawDebugCircle(player.position, 150.0f, 20, (HMM_Vec3){1.0f, 0.0f, 0.0f});
    if (bombShadow.isVisible) {
        drawDebugSquare(bombShadowPosition, 100.0f, (HMM_Vec3){0.0f, 1.0f, 0.0f});
    }
    drawDebugRect(handBomb.position, 80.0f, 60.0f, (HMM_Vec3){0.0f, 0.0f, 1.0f});
    flushDebugShapes();*/

    if(1.0f/state.deltaTime < 40.0f) {
        printf("Warning: FPS is low! Current FPS: %f\n", 1.0f/state.deltaTime);
    }
}

void camera_update(double mousePosX, double mousePosY)
{   
    HMM_Vec2 projDims = getProjectionDimensions();
    HMM_Vec2 playerWorld = (HMM_Vec2){(player.position.X - 0.5f*projDims.X), (player.position.Y - 0.5f*projDims.Y)};
    HMM_Vec2 mouseWorld = mouseToWorld(mousePosX, mousePosY);
    state.camX = playerWorld.X + (mouseWorld.X - (projDims.X/2.0f))/8.0f;
    state.camY = playerWorld.Y + (mouseWorld.Y - (projDims.Y/2.0f))/8.0f;
}

void move_update()
{   
    float speed = 10.0f;
    if (glfwGetKey(state.window, GLFW_KEY_W) == GLFW_PRESS)
    {   
        player.position.Y -= speed;
        if(checkCSCollision((HMM_Vec2){0.7f, 0.7f}, (HMM_Vec2){0.5f, 0.5f}, player.position, 0.1f)) {
            player.position.Y += speed;
        }
        camera_update(state.mouseX, state.mouseY);
    }
    if (glfwGetKey(state.window, GLFW_KEY_S) == GLFW_PRESS)
    {   
        player.position.Y += speed;
        if(checkCSCollision((HMM_Vec2){0.7f, 0.7f}, (HMM_Vec2){0.5f, 0.5f}, player.position, 0.1f)) {
            player.position.Y -= speed;
        }
        camera_update(state.mouseX, state.mouseY);
    }
    if (glfwGetKey(state.window, GLFW_KEY_A) == GLFW_PRESS)
    {
        player.position.X -= speed;
        if(checkCSCollision((HMM_Vec2){0.7f, 0.7f}, (HMM_Vec2){0.5f, 0.5f}, player.position, 0.1f)) {
            player.position.X += speed;
        }
        camera_update(state.mouseX, state.mouseY);
    }
    if (glfwGetKey(state.window, GLFW_KEY_D) == GLFW_PRESS)
    {
        player.position.X += speed;
        if(checkCSCollision((HMM_Vec2){0.7f, 0.7f}, (HMM_Vec2){0.5f, 0.5f}, player.position, 0.1f)) {
            player.position.X -= speed;
        }
        camera_update(state.mouseX, state.mouseY);
    }
}

void input(GLFWwindow* window, int key, int scancode, int action, int mods)
{   
    (void)scancode; // Unused parameter
    (void)mods; // Unused parameter
    (void)window; // Unused parameter

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
                if(bomb.isVisible == 0) {
                    bomb.position = (HMM_Vec2){player.position.X, player.position.Y};
                    bombShadowPosition = (HMM_Vec2){player.position.X, player.position.Y};
                    bomb.isVisible = 1;
                    HMM_Vec2 projDims = getProjectionDimensions();
                    HMM_Vec2 mouseWorld = mouseToWorld(state.mouseX, state.mouseY);
                    normDir = HMM_NormV2((HMM_Vec2){projDims.X/2.0f - mouseWorld.X, projDims.Y/2.0f - mouseWorld.Y});
                    initialTime = state.time;
                    startEmitter(&bomb.particleEmitter);
                } else {   
                    HMM_Vec2 projDims = getProjectionDimensions();
                    HMM_Vec2 mouseWorld = mouseToWorld(state.mouseX, state.mouseY);
                    normDirHandBomb = HMM_NormV2((HMM_Vec2){projDims.X/2.0f - mouseWorld.X, projDims.Y/2.0f - mouseWorld.Y});
                    bomb.isVisible = 0;
                    stopEmitter(&bomb.particleEmitter);
                }
                break;
            case GLFW_KEY_E:
                // Trigger explosion at player position
                explosionEmitter.position = player.position;
                emitBurst(&explosionEmitter, 40);
                break;
            case GLFW_KEY_R:
                // Toggle smoke emitter on/off
                if (smokeEmitter.isActive) {
                    stopEmitter(&smokeEmitter);
                } else {
                    smokeEmitter.position = player.position;
                    startEmitter(&smokeEmitter);
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
    (void)window; // Unused parameter
    
    state.mouseX = xposIn;
    state.mouseY = yposIn;
    camera_update(xposIn, yposIn);
}

int main(int argc, char** argv)
{   
    (void)argc; // Unused parameter
    (void)argv; // Unused parameter

    InitializeWindow(start, game_update, input, ui_update);
    return 0;
}
