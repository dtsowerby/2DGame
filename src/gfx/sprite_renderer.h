#pragma once

#include <HandmadeMath.h>

struct Entity;
struct Texture;

HMM_Vec2 screenToWorld(HMM_Vec2 position, HMM_Vec2 scale);
void drawSprite(int shaderProgram, Texture* texture, HMM_Vec2 position, HMM_Vec2 scale, float rotate, HMM_Vec3 colour);
void initSpriteRenderer();
void drawEntity(struct Entity* entity);
void drawTile(Tilemap* tilemap, int tileID, HMM_Vec2 position, HMM_Vec3 colour, int isFlipped, int shaderProgram);
