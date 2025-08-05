#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifdef __EMSCRIPTEN__
#define RESOURCES(x) "res-web/" #x
#else
#define RESOURCES(x) "res/" #x
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <glad.h>
#include <GLFW/glfw3.h>
#include <HandmadeMath.h>

#include <AL/al.h>
#include <AL/alc.h>

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
Entity projectile;
Entity bomb;
Entity enemy;
Entity enemyProjectile;
Entity item;

Sound song;
Sound coinCollect;
Sound finishStage;
Sound bought;
Sound shoot;
Sound gameEnd;

unsigned int tileShaderProgram;
unsigned int particleShaderProgram;
unsigned int backgroundShaderProgram;

PostProcessor postProcessor;

ParticleEmitter explosionEmitter;

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
EntityList* projectileEntities;
EntityList* enemyProjectileEntities;
EntityList* coinEntities;

// This is evil
EntityList* segments;

// Map dimensions: 8x8
// Tile dimensions: 16x16
Tilemap tilemap;

float col_one;
float col_two;
float col_three;

float wheel_colour;
float last_colour = -1.0f;

int segmentCount = 0;

unsigned int switchingScene = 0;
float timeSinceSwitchStart = 0.0f;
GameState nextState;

float timeSinceDeath = 0.0f;

unsigned int coins = 16;

unsigned int waves[] = {1, 2, 3, 5, 7, 12, 16, 24, 36, 52, 100, 248};
unsigned int enemyCount = 0;

float lastShot = 0.0f;

float powerUp = 1.0f;

//Things that use globals
#include "main.h"

void start()
{   
    printf("Starting game initialization...\n");
    
    state.tutorial = 0;

    state.gameState = STATE_MENU;
    state.stage = 1;
    state.stageCleared = 0;
    state.highScore = 1;

    printf("Initializing sprite renderer...\n");
    initSpriteRenderer();
    printf("Initializing debug shapes...\n");
    initDebugShapes();
    printf("Initializing primitives...\n");
    initPrimitives();

    bombEntities = createEntityList(100);
    enemyEntities = createEntityList(2000);
    projectileEntities = createEntityList(500);
    enemyProjectileEntities = createEntityList(200);
    coinEntities = createEntityList(200);

    segments = createEntityList(400);

    tileShaderProgram = createShaderProgramS(RESOURCES(shaders/sprite.vert), RESOURCES(shaders/tile.frag));
    particleShaderProgram = createShaderProgramS(RESOURCES(shaders/particle.vert), RESOURCES(shaders/particle.frag));
    backgroundShaderProgram = createShaderProgramS(RESOURCES(shaders/background.vert), RESOURCES(shaders/background.frag));

    initPostProcessor(&postProcessor, state.windowWidth, state.windowHeight);

    tilemap.texture = loadTexture(RESOURCES(art/tiles1.0.png));
    tilemap.tileWidth = 16;
    tilemap.tileCountX = 15;

    explosionEmitter = createExplosionEmitter((HMM_Vec2){0.5f, 0.5f}, 50);

    player.position = (HMM_Vec2){500.0f, 500.0f};
    player.scale = (HMM_Vec2){200.0f, 200.0f};
    player.colour = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    player.rotation = 0.0f;
    player.isVisible = 1;
    player.health = 10;

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

    projectile.shaderProgram = particleShaderProgram;
    projectile.scale = (HMM_Vec2){100.0f, 100.0f};
    projectile.rotation = 0.0f;
    projectile.isVisible = 1;

    item.shaderProgram = particleShaderProgram;
    item.scale = (HMM_Vec2){100.0f, 100.0f};
    item.rotation = 0.0f;
    item.isVisible = 1;

    enemyProjectile.shaderProgram = particleShaderProgram;
    enemyProjectile.scale = (HMM_Vec2){100.0f, 100.0f};
    enemyProjectile.rotation = 0.0f;
    enemyProjectile.isVisible = 1;

    enemy.shaderProgram = particleShaderProgram;
    enemy.scale = (HMM_Vec2){150.0f, 150.0f};
    enemy.colour = (HMM_Vec3){1.0f, 0.5f, 0.5f}; // Red tinted
    enemy.rotation = 0.0f;
    enemy.isVisible = 1;

    printf("Loading sounds...\n");
    song = loadSound(RESOURCES(sounds/Game1.wav));
    // = loadSound("res/sounds/projectilehit.wav");
    shoot = loadSound(RESOURCES(sounds/laser1.wav));
    coinCollect = loadSound(RESOURCES(sounds/tone1.wav));
    finishStage = loadSound(RESOURCES(sounds/spaceTrash1.wav));
    bought = loadSound(RESOURCES(sounds/threeTone1.wav));
    gameEnd = loadSound(RESOURCES(sounds/gameEnd.wav));
    playSoundLoop(song);

    //map = parseMapFile("res/maps/test.map");
    map.height = 16;
    map.width = 13;

    init_random();

    col_one = random_range(0.3f, 0.7f);
    col_two = random_range(0.3f, 0.7f);
    col_three = random_range(0.3f, 0.7f);
    
    printf("Game initialization complete!\n");
}

void spawnEnemy(HMM_Vec2 position, EnemyType type, HMM_Vec3 colour)
{   
    if(isEntityListFull(enemyEntities)){
            return;
    }
    Entity newEnemy = instantiateEntity(&enemy);
    newEnemy.position = position;
    newEnemy.enemyType = type;
    newEnemy.enemyColour = colour;
    newEnemy.particleEmitter = createEnemyEmitter(newEnemy.position, colour, 50);
    switch(type)
    {
        case ENEMY_TYPE_BASIC:
            newEnemy.particleEmitter.colorStart = (HMM_Vec3){1.0f, 0.0f, 0.0f};
            break;
        case ENEMY_TYPE_FAST:
            newEnemy.particleEmitter.colorStart = (HMM_Vec3){0.0f, 1.0f, 0.0f};
            break;
        case ENEMY_TYPE_TANK:
            newEnemy.particleEmitter.colorStart = (HMM_Vec3){0.0f, 0.0f, 1.0f};
            break;
        case ENEMY_TYPE_SHOOT:
            newEnemy.particleEmitter.colorStart = (HMM_Vec3){1.0f, 0.0f, 1.0f};
            break;
        case ENEMY_TYPE_BOSS:
            newEnemy.particleEmitter.colorStart = (HMM_Vec3){1.0f, 1.0f, 1.0f};
            newEnemy.particleEmitter.colorEnd = (HMM_Vec3){0.8f, 0.8f, 0.8f};
            break;
    }
    newEnemy.isVisible = 1;
    newEnemy.health = 2;
    newEnemy.timeAfterInstantiation = state.time;
    if(newEnemy.enemyType == ENEMY_TYPE_TANK) {
        newEnemy.particleEmitter.emissionRate = 25.0f;
        newEnemy.particleEmitter.scaleEnd = 1.5f;
        newEnemy.health = 5;
    }
    else if(newEnemy.enemyType == ENEMY_TYPE_BOSS) {
        newEnemy.particleEmitter.emissionRate = 25.0f;
        newEnemy.particleEmitter.scaleEnd = 2.0f;
        newEnemy.health = 15;
    }
    pushBack(enemyEntities, newEnemy);
    startEmitter(&newEnemy.particleEmitter);
    enemyCount++;
}

void spawnEnemies()
{
    int enemyCountt = 2 + state.stage * state.stage;
    int enemies[] = {ENEMY_TYPE_BASIC,ENEMY_TYPE_BASIC,ENEMY_TYPE_BASIC,ENEMY_TYPE_BASIC,ENEMY_TYPE_BASIC, ENEMY_TYPE_FAST, ENEMY_TYPE_FAST, ENEMY_TYPE_FAST, ENEMY_TYPE_TANK, ENEMY_TYPE_TANK, ENEMY_TYPE_SHOOT, ENEMY_TYPE_FAST, ENEMY_TYPE_BOSS};
    for(int i = 0; i < enemyCountt; i++) {
        float fmwefpwemf = (float)round(random_range(0, 1));
        HMM_Vec3 col1 = (HMM_Vec3){(float)round(1-fmwefpwemf), 0.0f, (float)round(fmwefpwemf)};
        HMM_Vec3 col2 = (HMM_Vec3){(float)round(fmwefpwemf), 0.0f, (float)round(1-fmwefpwemf)};
        spawnEnemy((HMM_Vec2){1100.0f + random_range(-100.0f, 100.0f), 1180.0f + random_range(-100.0f, 10.0f)}, enemies[i%sizeof(enemies)], col1);
        spawnEnemy((HMM_Vec2){1100.0f + random_range(-100.0f, 100.0f), 70.0f + random_range(-100.0f, 100.0f)}, enemies[(i + 1)%sizeof(enemies)], col2);
        spawnEnemy((HMM_Vec2){-50.0f + random_range(-100.0f, 100.0f), 70.0f + random_range(-100.0f, 100.0f)}, enemies[(i + 2)%sizeof(enemies)], col1);
        spawnEnemy((HMM_Vec2){-50.0f + random_range(-100.0f, 100.0f), 1180.0f + random_range(-100.0f, 100.0f)}, enemies[(i + 3)%sizeof(enemies)], col2);
    }
}

void game_update()
{   
    //Background
    postProcessor.chromaticAberrationStrength = 0.01f;
    postProcessor.vignetteStrength= 2.0f;
    HMM_Vec2 proj = getProjectionDimensions();
    drawBackground((HMM_Vec2){player.position.X, player.position.Y}, state.time/50.0f, backgroundShaderProgram);

    if(state.gameState == STATE_MENU) {
        // Reset
        clearEntityList(bombEntities);
        clearEntityList(enemyEntities);
        clearEntityList(projectileEntities);
        clearEntityList(enemyProjectileEntities);
        clearEntityList(coinEntities);
        clearEntityList(segments);
        segmentCount = 0;
        
        
        //enemyCount = 0;
        //lastShot = 0.0f;
        //timeSinceSwitchStart = 0.0f;
        //switchingScene = 0;
        
        // Reset wave counts to original values
        waves[0] = 1; waves[1] = 2; waves[2] = 3; waves[3] = 5; 
        waves[4] = 7; waves[5] = 12; waves[6] = 16; waves[7] = 24;
        waves[8] = 36; waves[9] = 52; waves[10] = 100; waves[11] = 248;
       // waves = (unsigned int[]){1, 2, 3, 5, 7, 12, 16, 24, 36, 52, 100, 248};
        // Reset player position
        //player.position = (HMM_Vec2){500.0f, 500.0f};
        
        addSegment(segments, (HMM_Vec3){1.0f, 1.0f, 1.0f});
        addSegment(segments, (HMM_Vec3){0.9f, 0.1f, 0.1f});
        addSegment(segments, (HMM_Vec3){0.0f, 0.0f, 0.0f});
        addSegment(segments, (HMM_Vec3){0.1f, 0.1f, 0.9f});
        state.stage = 1;
        state.stageCleared = 0;
        player.health = 10;
        coins = 16;
        timeSinceDeath = 0.0f;
        
        float textWidth = 850.0f;
        float scrollSpeed = 150.0f;
        float baseOffset = fmod(state.time * scrollSpeed, textWidth);
        for(int i = -1; i <= 2; i++) {
            float xPos = (float)player.position.X - textWidth + baseOffset + (i * textWidth);
            drawString(upheaval, "Revolver", (HMM_Vec2){xPos, 310.0f + player.position.Y - 500.0f}, (HMM_Vec3){1.0f, 1.0f, 1.0f}, 4.0f);
        }
        textWidth = 500.0f;
        scrollSpeed = 250.0f;
        baseOffset = fmod(state.time * scrollSpeed, textWidth);
        for(int i = -1; i <= 2; i++) {
            float xPos = (float)player.position.X - textWidth + baseOffset + (i * textWidth);
            drawString(upheaval, "Press Enter to start", (HMM_Vec2){xPos, (float)player.position.Y + 75.0f}, (HMM_Vec3){1.0f, 1.0f, 1.0f}, 1.0f);
        }
        if(state.highScore != 1)
        {   
            char highScoreText[64];
            textWidth = 800.0f;
            scrollSpeed = 250.0f;
            baseOffset = fmod(state.time * scrollSpeed, textWidth);
            sprintf(highScoreText, "High Score: %u", state.highScore);
            for(int i = -1; i <= 2; i++) {
                float xPos = (float)player.position.X - textWidth + baseOffset + (i * textWidth);
                drawString(upheaval, highScoreText, (HMM_Vec2){xPos, (float)player.position.Y - 250.0f}, (HMM_Vec3){1.0f, 0.0f, 0.0f}, 1.8f);
            }
        }
        if(glfwGetKey(state.window, GLFW_KEY_ENTER) == GLFW_PRESS) {
            if(state.tutorial == 0) {
                timeSinceSwitchStart = 0.0f;
                switchingScene = 1;
                nextState = STATE_TUTORIAL;
            } else {
                timeSinceSwitchStart = 0.0f;
                switchingScene = 1;
                nextState = STATE_CUSTOMIZE;
            }
        }
    } else if(state.gameState == STATE_CUSTOMIZE) {
        drawCircle((HMM_Vec2){(float)player.position.X, (float)player.position.Y-25.0f}, 250.0f, (HMM_Vec3){0.0f, 0.0f, 0.0f}, particleShaderProgram);
        for(int i = 0; i < getEntityListSize(segments); i++)
        {   
            Entity* segment = getEntityAtIndex(segments, i);
            //segment->scale.X = (float)fmod(segment->scale.X + state.deltaTime * 8.0f, 6.28f);
            drawCircleSegment((HMM_Vec2){(float)player.position.X + (float)sin(state.time * 3.0f) * (i%2) * 3.0f, (float)player.position.Y-25.0f + (float)cos(state.time * 2.0f) * ((i+1)%2) * 3.0f}, 240.0f, segment->scale.X+state.time/10.0f, segment->scale.Y, segment->colour, particleShaderProgram);
        }

        char vignetteText[64];
        char chromaticText[64];
        char healthText[64];
        sprintf(vignetteText, "Stage: %u", state.stage);
        sprintf(chromaticText, "Coins: %u", coins);
        sprintf(healthText, "Health: %u", player.health);
        drawString(upheaval, vignetteText, (HMM_Vec2){(float)player.position.X - 450.0f, (float)player.position.Y - 350.0f}, (HMM_Vec3){0.0f, 0.0f, 1.0f}, 1.8f);
        drawString(upheaval, chromaticText, (HMM_Vec2){(float)player.position.X - 450.0f, (float)player.position.Y - 350.0f + 40.0f}, (HMM_Vec3){1.0f, 1.0f, 0.0f}, 1.8f);
        drawString(upheaval, healthText, (HMM_Vec2){(float)player.position.X - 450.0f, (float)player.position.Y - 350.0f + 80.0f}, (HMM_Vec3){1.0f, 0.0f, 0.0f}, 1.8f);

        drawString(upheaval, "$4 Add Red", (HMM_Vec2){(float)player.position.X - 440.0f, (float)player.position.Y + 310.0f - 70.0f}, (HMM_Vec3){1.0f, 0.8f, 0.8f}, 1.3f);
        drawString(upheaval, "Press 1 to Buy", (HMM_Vec2){(float)player.position.X - 385.0f, (float)player.position.Y + 300.0f}, (HMM_Vec3){1.0, 0.8f, 0.8f}, 0.8f);
        
        drawString(upheaval, "$4 Add Blue", (HMM_Vec2){(float)player.position.X - 160.0f, (float)player.position.Y + 310.0f - 70.0f}, (HMM_Vec3){0.8f, 0.8f, 1.0f}, 1.3f);
        drawString(upheaval, "Press 2 to Buy", (HMM_Vec2){(float)player.position.X - 100.0f, (float)player.position.Y + 300.0f}, (HMM_Vec3){1.0f, 1.0f, 1.0f}, 0.8f);
        drawString(upheaval, "Will be placed to the left of white", (HMM_Vec2){(float)player.position.X - 275.0f, (float)player.position.Y + 340.0f}, (HMM_Vec3){1.0f, 1.0f, 1.0f}, 0.8f);        

        drawString(upheaval, "$8 Add Black", (HMM_Vec2){(float)player.position.X + 140, (float)player.position.Y + 310.0f - 70.0f}, (HMM_Vec3){0.2f, 0.2f, 0.2f}, 1.3f);
        drawString(upheaval, "Press 3 to Buy", (HMM_Vec2){(float)player.position.X + 180, (float)player.position.Y + 300.0f}, (HMM_Vec3){1.0f, 1.0f, 1.0f}, 0.8f);

        // Looped text
        char scrollText[] = "Press E to Continue to next Stage   ";
        float textWidth = 1000.0f; // Approximate width of the repeated text
        float scrollSpeed = 250.0f;
        float baseOffset = fmod(state.time * scrollSpeed, textWidth);
        for(int i = -1; i <= 2; i++) {
            float xPos = (float)player.position.X - textWidth + baseOffset + (i * textWidth);
            drawString(upheaval, scrollText, (HMM_Vec2){xPos, (float)player.position.Y + 50.0f}, (HMM_Vec3){1.0f, 1.0f, 1.0f}, 1.2f);
        }
        scrollSpeed = 200.0f;
        float secondOffset = fmod(state.time * scrollSpeed + (textWidth * 0.5f), textWidth);
        for(int i = -1; i <= 2; i++) {
            float xPos = (float)player.position.X - textWidth + secondOffset + (i * textWidth);
            drawString(upheaval, scrollText, (HMM_Vec2){xPos, (float)player.position.Y + 100.0f}, (HMM_Vec3){1.0f, 1.0f, 1.0f}, 1.2f);
        }
        postProcessor.chromaticAberrationStrength = (float)sin(state.time)/300.0f;

    } else if (state.gameState == STATE_GAMEPLAY) {
        move_update();
        lastShot += state.deltaTime;
        checkWheel();
        powerUp = max(1.0f, powerUp - state.deltaTime * 0.2f);

        if(timeSinceSwitchStart > 2.0f && (int)state.time % 1 == 0 && waves[state.stage-1] > 0)
        {
            spawnEnemies();
            waves[state.stage-1]--;
        } else if (waves[state.stage-1] == 0 && getEntityListSize(enemyEntities) == 0){
            char stageText[64];
            sprintf(stageText, "Stage %u Cleared!", state.stage);
            // Looped text
            float textWidth = 550.0f; // Approximate width of the repeated text
            float scrollSpeed = 250.0f;
            float baseOffset = fmod(state.time * scrollSpeed, textWidth);
            for(int i = -1; i <= 2; i++) {
                float xPos = (float)player.position.X - textWidth + baseOffset + (i * textWidth);
                drawString(upheaval, stageText, (HMM_Vec2){xPos, (float)player.position.Y + 30.0f}, (HMM_Vec3){0.0f, 1.0f, 1.0f}, 1.5f);
            }
            float secondOffset = fmod(state.time * scrollSpeed + (textWidth * 0.5f), textWidth);
            for(int i = -1; i <= 2; i++) {
                float xPos = (float)player.position.X - textWidth + secondOffset + (i * textWidth);
                drawString(upheaval, "Press E to Continue", (HMM_Vec2){xPos, (float)player.position.Y + 110.0f}, (HMM_Vec3){0.0f, 1.0f, 1.0f}, 1.2f);
            }
            state.stageCleared = 1;
        }
        
        //printf("enemyCount: %d\n", enemyCount);

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
            
            float bombTime = state.time - bombClone->timeAfterInstantiation;
            HMM_Vec2 direction = (HMM_Vec2){cosf(bombClone->rotation), sinf(bombClone->rotation)};
            
            bombClone->position.X -= direction.X * state.deltaTime * 400.0f;
            bombClone->position.Y -= direction.Y * state.deltaTime * 400.0f;
            
            bombClone->position.Y -= sinf(bombTime * 15.0f) * 15.0f;

            // Temp ShadowPosition
            bombClone->scale.Y -= direction.Y * state.deltaTime * 400.0f;

            //drawDebugCircle(bombClone->position, 30.0f, 20, (HMM_Vec3){1.0f, 0.0f, 0.0f});

            if (sin(bombTime * 7.2) < 0.0f) {
                explosionEmitter.position = bombClone->position;
                emitBurst(&explosionEmitter, 30);
                bombClone->particleEmitter.isActive = 0;
                for(int j = 0; j < getEntityListSize(enemyEntities); j++)
                {
                    Entity* enemyClone = getEntityAtIndex(enemyEntities, j);
                    if (enemyClone == NULL) continue;

                    if (checkCCCollision(bombClone->position, 30, enemyClone->position, 40)) {
                        enemyClone->health -= 1.8f;
                        if (enemyClone->health <= 0) {
                            enemyClone->particleEmitter.position = enemyClone->position;
                            removeEntityAtIndex(enemyEntities, j);
                            enemyCount--;
                            j--;
                            Entity newItem = instantiateEntity(&item);
                            newItem.position = enemyClone->position;
                            newItem.itemType = round(max(0.0f, random_range(-1.0f, 1.0f))+max(0.0f, random_range(-1.0f, 1.0f)));
                            newItem.timeAfterInstantiation = state.time;
                            switch (newItem.itemType)
                            {
                            case ITEM_TYPE_COIN:
                                newItem.particleEmitter = createSmokeEmitter(newItem.position, (HMM_Vec3){1.0f, 1.0f, 0.0f});
                                break;
                            case ITEM_TYPE_HEALTH:
                                newItem.particleEmitter = createSmokeEmitter(newItem.position, (HMM_Vec3){1.0f, 0.0f, 0.0f});
                                break;
                            case ITEM_TYPE_POWERUP:
                                newItem.particleEmitter = createSmokeEmitter(newItem.position, (HMM_Vec3){1.0f, 0.0f, 1.0f});   
                                break;
                            case ITEM_TYPE_FREEZE:
                                newItem.particleEmitter = createSmokeEmitter(newItem.position, (HMM_Vec3){0.0f, 0.0f, 1.0f});
                                break;
                            default:
                                printf("Unknown item type: %i\n", newItem.itemType);
                                break;
                            }
                            pushBack(coinEntities, newItem);
                            startEmitter(&newItem.particleEmitter);
                        }
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

        for(int i = 0; i < getEntityListSize(coinEntities); i++) {
            Entity* coinClone = getEntityAtIndex(coinEntities, i);
            if (coinClone == NULL) continue;
            int playerCollision = checkCCCollision(coinClone->position, 20, player.position, 40);
            if(playerCollision) 
            {
            playSound(coinCollect);
            switch(coinClone->itemType) {
                case ITEM_TYPE_COIN:
                    coins++;
                    removeEntityAtIndex(coinEntities, i);
                    i--;
                    continue;
                    break;
                case ITEM_TYPE_HEALTH:
                    player.health++;
                    removeEntityAtIndex(coinEntities, i);
                    i--;
                    continue;
                    break;
                case ITEM_TYPE_POWERUP:
                    powerUp += 1.0f;
                    removeEntityAtIndex(coinEntities, i);
                    i--;
                    continue;
                    break;
                case ITEM_TYPE_FREEZE:
                    removeEntityAtIndex(coinEntities, i);
                    i--;
                    continue;
                    break;
                }
            }
            updateEntity(coinClone);
            drawEntity(coinClone);

        }

        // Projectile Behaviour
        for (int i = 0; i < getEntityListSize(projectileEntities); i++) {
            Entity* projectileClone = getEntityAtIndex(projectileEntities, i);
            if (projectileClone == NULL) continue;

            HMM_Vec2 direction = (HMM_Vec2){cosf(projectileClone->rotation), sinf(projectileClone->rotation)};

            projectileClone->position.X -= direction.X * state.deltaTime * 1200.0f;
            projectileClone->position.Y -= direction.Y * state.deltaTime * 1200.0f;

            //drawDebugCircle(projectileClone->position, 10.0f, 20, (HMM_Vec3){1.0f, 0.0f, 0.0f});

            for(int j = 0; j < getEntityListSize(enemyEntities); j++)
            {
                Entity* enemyClone = getEntityAtIndex(enemyEntities, j);
                if (enemyClone == NULL) continue;

                if (checkCCCollision(projectileClone->position, 10, enemyClone->position, 40)) {
                    //enemyClone->particleEmitter.position = enemyClone->position;;
                    //if(projectileClone->colour.X < enemyClone->enemyColour.X - 0.5f || projectileClone->colour.X > enemyClone->enemyColour.X + 0.5f) {continue;}
                    float initEnemyHealth = enemyClone->health;
                    enemyClone->health-=projectileClone->health;
                    projectileClone->health-= initEnemyHealth;
                    
                    //printf("Enemy Health: %f\n", enemyClone->health);
                    if (enemyClone->health <= 0) {
                        Entity newItem = instantiateEntity(&item);
                        newItem.position = enemyClone->position;
                        newItem.itemType = round(random_range(0, 1));
                        newItem.timeAfterInstantiation = state.time;
                        switch (newItem.itemType)
                        {
                        case ITEM_TYPE_COIN:
                            newItem.particleEmitter = createSmokeEmitter(newItem.position, (HMM_Vec3){1.0f, 1.0f, 0.0f});
                            break;
                        case ITEM_TYPE_HEALTH:
                            newItem.particleEmitter = createSmokeEmitter(newItem.position, (HMM_Vec3){1.0f, 0.0f, 0.0f});
                            break;
                        case ITEM_TYPE_POWERUP:
                            newItem.particleEmitter = createSmokeEmitter(newItem.position, (HMM_Vec3){1.0f, 0.0f, 1.0f});   
                            break;
                        case ITEM_TYPE_FREEZE:
                            newItem.particleEmitter = createSmokeEmitter(newItem.position, (HMM_Vec3){0.0f, 0.0f, 1.0f});
                            break;
                        default:
                            printf("Unknown item type: %i\n", newItem.itemType);
                            break;
                        }
                        pushBack(coinEntities, newItem);
                        startEmitter(&newItem.particleEmitter);
                        removeEntityAtIndex(enemyEntities, j);
                        enemyCount--;
                        j--;
                    }
                    
                    if(projectileClone->health <= 0) {
                        removeEntityAtIndex(projectileEntities, i);
                        i--;
                        break; // Break out of enemy loop since projectile is destroyed
                    }
                    continue;
                }
            }

            if (state.time - projectileClone->timeAfterInstantiation > 1.5f)
            {
                removeEntityAtIndex(projectileEntities, i);
                i--;
                continue;
            }
            
            updateEntity(projectileClone);
            drawEntity(projectileClone);
        }

        // EnemyProjectile Behaviour
        for (int i = 0; i < getEntityListSize(enemyProjectileEntities); i++) {
            Entity* enemyProjectileClone = getEntityAtIndex(enemyProjectileEntities, i);
            if (enemyProjectileClone == NULL) continue;

            HMM_Vec2 direction = (HMM_Vec2){cosf(enemyProjectileClone->rotation), sinf(enemyProjectileClone->rotation)};

            enemyProjectileClone->position.X -= direction.X * state.deltaTime * 500.0f;
            enemyProjectileClone->position.Y -= direction.Y * state.deltaTime * 500.0f;

                Entity* playerClone = &player;

            if (checkCCCollision(enemyProjectileClone->position, 10, playerClone->position, 40)) {
                removeEntityAtIndex(enemyProjectileEntities, i);
                i--;
                playerClone->health--;
                if(playerClone->health < 0) playerClone->health = 0;
                //printf("Player Health: %d\n", playerClone->health);
                if (playerClone->health <= 0) {
                    printf("Game Over\n");
                }
            }

            if (state.time - enemyProjectileClone->timeAfterInstantiation > 10.0f)
            {
                removeEntityAtIndex(enemyProjectileEntities, i);
                i--;
                continue;
            }

            updateEntity(enemyProjectileClone);
            drawEntity(enemyProjectileClone);
        }

        // Enemy Behaviour
        for (int i = 0; i < getEntityListSize(enemyEntities); i++) {
            Entity* enemyClone = getEntityAtIndex(enemyEntities, i);
            if (enemyClone == NULL) continue;

            HMM_Vec2 direction = HMM_NormV2((HMM_Vec2){player.position.X - enemyClone->position.X + random_range(-50.0f, 50.0f),
                                            player.position.Y - enemyClone->position.Y + random_range(-50.0f, 50.0f)});

            if (checkCCCollision(enemyClone->position, 40, player.position, 40)) {
                player.health--;
                if(player.health < 0) player.health = 0;
                //printf("Player Health: %d\n", player.health);
                explosionEmitter.position = player.position;
                emitBurst(&explosionEmitter, 40);
                removeEntityAtIndex(enemyEntities, i);
                enemyCount--;
                i--;
                continue;
            }

            if (enemyClone->enemyType == ENEMY_TYPE_TANK) {
                enemyClone->position.X += direction.X * state.deltaTime * 80.0f;
                enemyClone->position.Y += direction.Y * state.deltaTime * 80.0f;
            } else if (enemyClone->enemyType == ENEMY_TYPE_BASIC) {
                enemyClone->position.X += direction.X * state.deltaTime * 150.0f;
                enemyClone->position.Y += direction.Y * state.deltaTime * 150.0f;
            } else if(enemyClone->enemyType == ENEMY_TYPE_FAST) {
                enemyClone->position.X += direction.X * state.deltaTime * 300.0f;
                enemyClone->position.Y += direction.Y * state.deltaTime * 300.0f;
            } else if(enemyClone->enemyType == ENEMY_TYPE_SHOOT) {
                enemyClone->position.X += sin(state.time * 2.0f) * 200.0f * state.deltaTime;
                enemyClone->position.Y += cos(state.time * 2.0f) * 200.0f * state.deltaTime;
                
                // Shoot every 2 seconds using a more reliable timing method
                float timeSinceSpawn = state.time - enemyClone->timeAfterInstantiation;
                if (fmod(timeSinceSpawn, 2.0f) < state.deltaTime && timeSinceSpawn > 1.0f) {
                    Entity newProjectile = instantiateEntity(&enemyProjectile);
                    newProjectile.position = (HMM_Vec2){enemyClone->position.X, enemyClone->position.Y};
                    newProjectile.particleEmitter = createProjectileEmitter((HMM_Vec2){enemyClone->position.X, enemyClone->position.Y}, 50);
                    newProjectile.rotation = -atan2f(direction.Y, -direction.X);
                    newProjectile.timeAfterInstantiation = state.time;
                    setEmitterColorRange(&newProjectile.particleEmitter, (HMM_Vec3){0.0f, 0.0f, 0.0f}, (HMM_Vec3){0.8f, 0.1f, 0.1f});
                    setEmitterScaleRange(&newProjectile.particleEmitter, 0.5f, 1.0f);
                    pushBack(enemyProjectileEntities, newProjectile);
                    startEmitter(&newProjectile.particleEmitter);
                }
            } else if(enemyClone->enemyType == ENEMY_TYPE_BOSS) {
                enemyClone->position.X += direction.X * state.deltaTime * 120.0f;
                enemyClone->position.Y += direction.Y * state.deltaTime * 100.0f;
                
                // Shoot every 3 seconds using a more reliable timing method
                float timeSinceSpawn = state.time - enemyClone->timeAfterInstantiation;
                if (fmod(timeSinceSpawn, 3.0f) < state.deltaTime && timeSinceSpawn > 1.0f) {
                    Entity newProjectile = instantiateEntity(&enemyProjectile);
                    newProjectile.position = (HMM_Vec2){enemyClone->position.X, enemyClone->position.Y};
                    newProjectile.particleEmitter = createProjectileEmitter((HMM_Vec2){enemyClone->position.X, enemyClone->position.Y}, 50);
                    newProjectile.rotation = -atan2f(direction.Y, direction.X);
                    newProjectile.timeAfterInstantiation = state.time;
                    setEmitterColorRange(&newProjectile.particleEmitter, (HMM_Vec3){0.0f, 0.0f, 0.0f}, (HMM_Vec3){0.8f, 0.1f, 0.1f});
                    setEmitterScaleRange(&newProjectile.particleEmitter, 0.5f, 1.0f);
                    pushBack(enemyProjectileEntities, newProjectile);
                    startEmitter(&newProjectile.particleEmitter);
                }
            }

            if(player.health <= 0) {
                updateParticleEmitter(&explosionEmitter);
                renderParticleEmitter(&explosionEmitter, particleShaderProgram);
                playSound(gameEnd);
                switchingScene = 1;
                timeSinceDeath = 0.0f;
                nextState = STATE_DEATH;
            }

            //drawDebugCircle(enemyClone->position, 40.0f, 20, enemyClone->enemyColour);

            updateEntity(enemyClone);
            drawEntity(enemyClone);

            //drawDebugCircle(enemyClone->position, 40.0f, 20, (HMM_Vec3){0.0f, 1.0f, 0.0f});
        }

        // Player Updates
        updateEntity(&player);
        drawEntity(&player);
        
        if(player.health <= 0) {
            updateParticleEmitter(&explosionEmitter);
            renderParticleEmitter(&explosionEmitter, particleShaderProgram);
            playSound(gameEnd);
            switchingScene = 1;
            timeSinceDeath = 0.0f;
            nextState = STATE_DEATH;
        }
        
        /*drawString(upheaval, "Hello World!", (HMM_Vec2){100.0f, 100.0f}, (HMM_Vec3){1.0f, 1.0f, 1.0f}, 5.0f);*/

        // Player
        drawCircle((HMM_Vec2){(float)player.position.X, (float)player.position.Y}, 40.0f, (HMM_Vec3){0.0f, 0.0f, 0.0f}, particleShaderProgram);
        for(int i = 0; i < getEntityListSize(segments); i++)
        {   
            Entity* segment = getEntityAtIndex(segments, i);
            segment->scale.X = (float)fmod(segment->scale.X + state.deltaTime * 8.0f, 6.28f);
            drawCircleSegment((HMM_Vec2){(float)player.position.X, (float)player.position.Y}, 45.0f, segment->scale.X, segment->scale.Y, segment->colour, particleShaderProgram);
        }
        drawRectPivot((HMM_Vec2){(float)player.position.X - 30.0f, (float)player.position.Y}, (HMM_Vec2){75.0f, 10.0f}, getMouseAngle(), (HMM_Vec2){30.0f, 0.0f}, (HMM_Vec3){0.0f, 0.0f, 0.0f}, particleShaderProgram);

        updateParticleEmitter(&explosionEmitter);

        renderParticleEmitter(&explosionEmitter, particleShaderProgram);

        char vignetteText[64];
        char chromaticText[64];
        char healthText[64];
        char powerUpText[64];
        sprintf(vignetteText, "Stage: %u", state.stage);
        sprintf(chromaticText, "Coins: %u", coins);
        sprintf(healthText, "Health: %u", player.health);
        sprintf(powerUpText, "Powerup: x%.2f", powerUp);
        drawString(upheaval, vignetteText, (HMM_Vec2){(float)player.position.X - 450.0f, (float)player.position.Y - 350.0f}, (HMM_Vec3){0.0f, 0.0f, 1.0f}, 1.8f);
        drawString(upheaval, chromaticText, (HMM_Vec2){(float)player.position.X - 450.0f, (float)player.position.Y - 350.0f + 40.0f}, (HMM_Vec3){1.0f, 1.0f, 0.0f}, 1.8f);
        drawString(upheaval, healthText, (HMM_Vec2){(float)player.position.X - 450.0f, (float)player.position.Y - 350.0f + 80.0f}, (HMM_Vec3){1.0f, 0.0f, 0.0f}, 1.8f);
        if(powerUp > 1.0f) {
            drawString(upheaval, powerUpText, (HMM_Vec2){(float)player.position.X - 450.0f, (float)player.position.Y - 350.0f + 120.0f}, (HMM_Vec3){1.0f, 0.0f, 1.0f}, 1.8f);
        }
    } else if (state.gameState == STATE_TUTORIAL) 
    {   
        postProcessor.chromaticAberrationStrength = 0.001f;
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
        move_update();
        lastShot += state.deltaTime;
        checkWheel();
        drawString(upheaval, "Tutorial\n\nWASD to move\nF to fullscreen\n\nShoots automatically when new\ncolour touches barrel\n\nYou can add new\ncolours in the shop\n\nPutting the same colour next\nto each other increases damage\nbut takes longer between firing\n\nPress Enter when ready to start", (HMM_Vec2){0.0f, 400.0f}, (HMM_Vec3){1.0f, 1.0f, 1.0f}, 1.2f);
        // Player
        drawCircle((HMM_Vec2){(float)player.position.X, (float)player.position.Y}, 40.0f, (HMM_Vec3){0.0f, 0.0f, 0.0f}, particleShaderProgram);
        for(int i = 0; i < getEntityListSize(segments); i++)
        {   
            Entity* segment = getEntityAtIndex(segments, i);
            segment->scale.X = (float)fmod(segment->scale.X + state.deltaTime * 8.0f, 6.28f);
            drawCircleSegment((HMM_Vec2){(float)player.position.X, (float)player.position.Y}, 45.0f, segment->scale.X, segment->scale.Y, segment->colour, particleShaderProgram);
        }
        drawRectPivot((HMM_Vec2){(float)player.position.X - 30.0f, (float)player.position.Y}, (HMM_Vec2){75.0f, 10.0f}, getMouseAngle(), (HMM_Vec2){30.0f, 0.0f}, (HMM_Vec3){0.0f, 0.0f, 0.0f}, particleShaderProgram);

        // Projectile Behaviour
        for (int i = 0; i < getEntityListSize(projectileEntities); i++) {
            Entity* projectileClone = getEntityAtIndex(projectileEntities, i);
            if (projectileClone == NULL) continue;

            HMM_Vec2 direction = (HMM_Vec2){cosf(projectileClone->rotation), sinf(projectileClone->rotation)};

            projectileClone->position.X -= direction.X * state.deltaTime * 1200.0f;
            projectileClone->position.Y -= direction.Y * state.deltaTime * 1200.0f;

            if (state.time - projectileClone->timeAfterInstantiation > 1.5f)
            {
                removeEntityAtIndex(projectileEntities, i);
                i--;
                continue;
            }
            
            updateEntity(projectileClone);
            drawEntity(projectileClone);
        }

        for (int i = 0; i < getEntityListSize(bombEntities); i++) {
            Entity* bombClone = getEntityAtIndex(bombEntities, i);
            if (bombClone == NULL) continue;
            
            float bombTime = state.time - bombClone->timeAfterInstantiation;
            HMM_Vec2 direction = (HMM_Vec2){cosf(bombClone->rotation), sinf(bombClone->rotation)};
            
            bombClone->position.X -= direction.X * state.deltaTime * 400.0f;
            bombClone->position.Y -= direction.Y * state.deltaTime * 400.0f;
            
            bombClone->position.Y -= sinf(bombTime * 15.0f) * 15.0f;
            bombClone->scale.Y -= direction.Y * state.deltaTime * 400.0f;

            if (sin(bombTime * 7.2) < 0.0f) {
                explosionEmitter.position = bombClone->position;
                emitBurst(&explosionEmitter, 30);
                bombClone->particleEmitter.isActive = 0;
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

        updateParticleEmitter(&explosionEmitter);

        renderParticleEmitter(&explosionEmitter, particleShaderProgram);

        if(glfwGetKey(state.window, GLFW_KEY_ENTER) == GLFW_PRESS) {
            timeSinceSwitchStart = 0.0f;
            switchingScene = 1;
            nextState = STATE_MENU;
            state.tutorial = 1;
        }
    } else if (state.gameState == STATE_DEATH)
    {   
        pauseSound(song);
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
        char deathText[64];
        sprintf(deathText, "  Game Over\nStage reached: %u", state.stage);
        drawString(upheaval, deathText, (HMM_Vec2){-480.0f + player.position.X, sin(state.time * 5.0f)*10.0f - 100.0f + player.position.Y}, (HMM_Vec3){1.0f, 1.0f, 1.0f}, 2.6f);
        if(timeSinceDeath > 2.5f) {
            if(nextState == STATE_DEATH) {timeSinceSwitchStart = 0.0f;}
            switchingScene = 1; 
            nextState = STATE_MENU; 
        }
        if(timeSinceDeath > 2.9f) {
            resumeSound(song);
        } 
        timeSinceDeath += state.deltaTime;
    }

    flushDebugShapes();
    flushFontBatch();
    
    timeSinceSwitchStart += state.deltaTime;
    if(switchingScene) 
    {   
        if(timeSinceSwitchStart >= 0.5f) {
            state.gameState = nextState;
        }
        if(timeSinceSwitchStart < 1.5f) {
            HMM_Vec2 projDims = getProjectionDimensions();
            drawRect((HMM_Vec2){- projDims.X + timeSinceSwitchStart*projDims.X*(1.0f/0.4f), player.position.Y}, (HMM_Vec2){projDims.X*1.25f, projDims.Y*1.5f}, 0.0f, (HMM_Vec3){0.0f, 0.0f, 0.0f}, particleShaderProgram);
        } else {switchingScene = 0; lastShot = 0.0f;}
    }

    /*if(1.0f/state.deltaTime < 40.0f) {
        //printf("FPS below 40: %f\n", 1.0f/state.deltaTime);
    }*/

    //printf("Player Position: (%f, %f)\n", player.position.X, player.position.Y);
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
        if(player.position.Y <= -50.0f) {
            player.position.Y += speed;
        }
        camera_update(state.mouseX, state.mouseY);
    }
    if (glfwGetKey(state.window, GLFW_KEY_S) == GLFW_PRESS)
    {   
        player.position.Y += speed;
        if(player.position.Y >= 1325.0f) {
            player.position.Y -= speed;
        }
        camera_update(state.mouseX, state.mouseY);
    }
    if (glfwGetKey(state.window, GLFW_KEY_A) == GLFW_PRESS)
    {
        player.position.X -= speed;
        if(player.position.X <= -50.0f) {
            player.position.X += speed;
        }
        camera_update(state.mouseX, state.mouseY);
    }
    if (glfwGetKey(state.window, GLFW_KEY_D) == GLFW_PRESS)
    {
        player.position.X += speed;
        if(player.position.X >= 1075.0f) {
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
            case GLFW_KEY_F:
                toggleFullscreen(state.window);
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
                //printf("enemyCount: %d\n", enemyCount);
                break;
            case GLFW_KEY_E:
                if(state.stageCleared == 1) {
                    timeSinceSwitchStart = 0.0f;
                    switchingScene = 1;
                    nextState = (state.gameState == STATE_GAMEPLAY) ? STATE_CUSTOMIZE : STATE_GAMEPLAY;
                    state.stageCleared = 0;
                    state.stage++;
                    state.highScore = max(state.highScore, state.stage);
                    playSound(finishStage);
                } else if (state.gameState == STATE_CUSTOMIZE) {
                    timeSinceSwitchStart = 0.0f;
                    switchingScene = 1;
                    nextState = STATE_GAMEPLAY;
                    playSound(finishStage);
                    col_one = random_range(0.3f, 0.7f);
                    col_two = random_range(0.3f, 0.7f);
                    col_three = random_range(0.3f, 0.7f);
                }
                break;
            }
        if(state.gameState == STATE_CUSTOMIZE) {
            switch(key){
            case GLFW_KEY_2:
                if(coins >= 4)
                {
                    addSegment(segments, (HMM_Vec3){0.1f, 0.1f, 0.9f});
                    coins-=4;
                    playSound(bought);
                }
                break;
            case GLFW_KEY_1:
                if(coins >= 4)
                {
                    addSegment(segments, (HMM_Vec3){0.9f, 0.1f, 0.1f});
                    coins-=4;
                    playSound(bought);
                }
                break;
            case GLFW_KEY_3:
                if(coins >= 8)
                {
                    addSegment(segments, (HMM_Vec3){0.0f, 0.0f, 0.0f});
                    coins-=8;
                    playSound(bought);
                }
                break;
            }
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

#ifdef __EMSCRIPTEN__
    printf("Emscripten main called, initializing...\n");
    // Emscripten will handle dependency loading automatically
    InitializeWindow(start, game_update, input, ui_update);
    return 0;
#else
    InitializeWindow(start, game_update, input, ui_update);
    return 0;
#endif
}
