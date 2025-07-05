#pragma once

#include <HandmadeMath.h>

struct Entity;
struct Texture;

void drawSprite(int shaderProgram, Texture texture, HMM_Vec2 position, HMM_Vec2 scale, float rotate, HMM_Vec3 colour);
void initSpriteRenderer();
void drawEntity(struct Entity entity);
void drawTileEntity(struct Entity entity, int tileID);
