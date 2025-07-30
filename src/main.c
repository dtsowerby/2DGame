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
#include "utils/memory.h"

#include "gfx/shader.h"
#include "gfx/texture.h"
#include "gfx/sprite_renderer.h"
#include "gfx/particle.h"
#include "gfx/primitive.h"
#include "gfx/font.h"
#include "gfx/postprocess.h"

#include "game/collision.h"

#include "entity.h"

#include "state.h"

void move_update();

State state;

Entity player;
Entity bomb;
Entity enemy;

Sound explode;
Sound bugle;

unsigned int tileShaderProgram;
unsigned int particleShaderProgram;

PostProcessor postProcessor;

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

EntityList* bombEntities;
EntityList* enemyEntities;

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

    bombEntities = createEntityList(100);
    enemyEntities = createEntityList(100);

    tileShaderProgram = createShaderProgramS("res/shaders/sprite.vert", "res/shaders/tile.frag");
    particleShaderProgram = createShaderProgramS("res/shaders/particle.vert", "res/shaders/particle.frag");

    initPostProcessor(&postProcessor, state.windowWidth, state.windowHeight);
    
    tilemap.texture = loadTexture("res/art/tiles1.0.png");
    tilemap.tileWidth = 16;
    tilemap.tileCountX = 15;

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

    player.position = (HMM_Vec2){500.0f, 500.0f};
    player.scale = (HMM_Vec2){200.0f, 200.0f};
    player.colour = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    player.rotation = 0.0f;
    player.isVisible = 1;

    bomb.shaderProgram = tileShaderProgram;
    bomb.scale = (HMM_Vec2){200.0f, 200.0f};
    bomb.colour = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    bomb.rotation = 0.0f;
    bomb.isVisible = 1;

    enemy.shaderProgram = particleShaderProgram;
    enemy.scale = (HMM_Vec2){150.0f, 150.0f};
    enemy.colour = (HMM_Vec3){1.0f, 0.5f, 0.5f}; // Red tinted
    enemy.rotation = 0.0f;
    enemy.isVisible = 1;

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
                (HMM_Vec2){((float)j)*state.tileDim + (float)sin(state.time * i) * 4.0f, 
                ((float)i)*state.tileDim - 0.6f + (float)cos(state.time * j) * 4.0f}, 
                (HMM_Vec3){(float)fabs((i+j)%2 - col_one),
                           (float)fabs((i+j)%2 - col_two), 
                           (float)fabs((i+j)%2 - col_three)}, 0,  particleShaderProgram 
            );
        }
    }
    
    // Bomb Behaviour
    for (int i = 0; i < getEntityListSize(bombEntities); i++) {
        Entity* bombClone = getEntityAtIndex(bombEntities, i);
        if (bombClone == NULL) continue;
        
        float bombTime = state.time - bombClone->timeAfterBomb;
        HMM_Vec2 direction = (HMM_Vec2){cosf(bombClone->rotation), sinf(bombClone->rotation)};
        
        bombClone->position.X -= direction.X * state.deltaTime * 400.0f;
        bombClone->position.Y -= direction.Y * state.deltaTime * 400.0f;
        
        bombClone->position.Y -= sinf(bombTime * 15.0f) * 15.0f;

        // Temp ShadowPosition
        bombClone->scale.Y -= direction.Y * state.deltaTime * 400.0f;

        drawDebugCircle(bombClone->position, 30.0f, 20, (HMM_Vec3){1.0f, 0.0f, 0.0f});

        if (sin(bombTime * 7.2) < 0.0f) {
            explosionEmitter.position = bombClone->position;
            emitBurst(&explosionEmitter, 30);
            bombClone->particleEmitter.isActive = 0;
            for(int j = 0; j < getEntityListSize(enemyEntities); j++)
            {
                Entity* enemyClone = getEntityAtIndex(enemyEntities, j);
                if (enemyClone == NULL) continue;

                if (checkCCCollision(bombClone->position, 30, enemyClone->position, 40)) {
                    enemyClone->particleEmitter.position = enemyClone->position;;
                    removeEntityAtIndex(enemyEntities, j);
                    j--;
                }
            }
            removeEntityAtIndex(bombEntities, i);
            i--;
            continue;
        }
        
        updateEntity(bombClone);
        drawEntity(bombClone);
        
        HMM_Vec2 shadowPos = (HMM_Vec2){bombClone->position.X, bombClone->scale.Y }; // Shadow offset
        drawSquare(shadowPos, 20.0f, 45.0f, (HMM_Vec3){0.0f, 0.0f, 0.0f}, particleShaderProgram);
        drawSquare(shadowPos, 18.0f, 135.0f, (HMM_Vec3){0.0f, 0.0f, 0.0f}, particleShaderProgram);
    }

    // Enemy Behaviour
    for (int i = 0; i < getEntityListSize(enemyEntities); i++) {
        Entity* enemyClone = getEntityAtIndex(enemyEntities, i);
        if (enemyClone == NULL) continue;

        HMM_Vec2 direction = HMM_NormV2((HMM_Vec2){player.position.X - enemyClone->position.X, 
                                         player.position.Y - enemyClone->position.Y});
        enemyClone->position.X += direction.X * state.deltaTime * 100.0f;
        enemyClone->position.Y += direction.Y * state.deltaTime * 100.0f;

        updateEntity(enemyClone);
        drawEntity(enemyClone);

        drawDebugCircle(enemyClone->position, 40.0f, 20, (HMM_Vec3){0.0f, 1.0f, 0.0f});
    }

    // Player Updates
    updateEntity(&player);
    drawEntity(&player);
    
    // Environment Updates
    updateParticleEmitter(&explosionEmitter);
    updateParticleEmitter(&smokeEmitter);

    renderParticleEmitter(&explosionEmitter, particleShaderProgram);
    renderParticleEmitter(&smokeEmitter, particleShaderProgram);
    
    drawString(upheaval, "Hello World!", (HMM_Vec2){100.0f, 100.0f}, (HMM_Vec3){1.0f, 1.0f, 1.0f}, 5.0f);
    drawString(upheaval, "Batch Rendering!", (HMM_Vec2){100.0f, 150.0f}, (HMM_Vec3){1.0f, 0.5f, 0.0f}, 1.0f);
    
    // Post-processing controls UI
    char vignetteText[64];
    char chromaticText[64];
    sprintf(vignetteText, "Vignette (1/2): %.2f", postProcessor.vignetteStrength);
    sprintf(chromaticText, "Chromatic (3/4): %.4f", postProcessor.chromaticAberrationStrength);
    
    drawString(upheaval, vignetteText, (HMM_Vec2){20.0f, 20.0f}, (HMM_Vec3){0.8f, 0.8f, 1.0f}, 0.8f);
    drawString(upheaval, chromaticText, (HMM_Vec2){20.0f, 50.0f}, (HMM_Vec3){0.8f, 0.8f, 1.0f}, 0.8f);
    drawString(upheaval, "Post-Processing Demo", (HMM_Vec2){20.0f, 80.0f}, (HMM_Vec3){1.0f, 1.0f, 0.0f}, 1.0f);

    if(1.0f/state.deltaTime < 40.0f) {
        printf("FPS below 40: %f\n", 1.0f/state.deltaTime);
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
                //cleanupPostProcessor(&postProcessor);
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
            {
                Entity newBomb = instantiateEntity(&bomb);
                newBomb.position = (HMM_Vec2){player.position.X, player.position.Y};
                newBomb.scale = newBomb.position;
                newBomb.particleEmitter = createBombEmitter((HMM_Vec2){0.5f, 0.5f}, 50);
                newBomb.timeAfterBomb = state.time;
                
                HMM_Vec2 projDims = getProjectionDimensions();
                HMM_Vec2 mouseWorld = mouseToWorld(state.mouseX, state.mouseY);
                HMM_Vec2 direction = HMM_NormV2((HMM_Vec2){projDims.X/2.0f - mouseWorld.X, projDims.Y/2.0f - mouseWorld.Y});
                newBomb.rotation = atan2f(direction.Y, direction.X);
                
                pushBack(bombEntities, newBomb);
                startEmitter(&newBomb.particleEmitter);
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
            case GLFW_KEY_F:
                // Instantiate Enemy
                Entity newEnemy = instantiateEntity(&enemy);
                newEnemy.position = player.position;
                newEnemy.particleEmitter = createEnemyEmitter(newEnemy.position, 50);
                pushBack(enemyEntities, newEnemy);
                startEmitter(&newEnemy.particleEmitter);
                break;
            case GLFW_KEY_1:
                // Decrease vignette strength
                postProcessor.vignetteStrength = HMM_MAX(0.0f, postProcessor.vignetteStrength - 0.1f);
                printf("Vignette strength: %.2f\n", postProcessor.vignetteStrength);
                break;
            case GLFW_KEY_2:
                // Increase vignette strength  
                postProcessor.vignetteStrength = HMM_MIN(2.0f, postProcessor.vignetteStrength + 0.1f);
                printf("Vignette strength: %.2f\n", postProcessor.vignetteStrength);
                break;
            case GLFW_KEY_3:
                // Decrease chromatic aberration
                postProcessor.chromaticAberrationStrength = HMM_MAX(0.0f, postProcessor.chromaticAberrationStrength - 0.001f);
                printf("Chromatic aberration: %.4f\n", postProcessor.chromaticAberrationStrength);
                break;
            case GLFW_KEY_4:
                // Increase chromatic aberration
                postProcessor.chromaticAberrationStrength = HMM_MIN(0.01f, postProcessor.chromaticAberrationStrength + 0.001f);
                printf("Chromatic aberration: %.4f\n", postProcessor.chromaticAberrationStrength);
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
