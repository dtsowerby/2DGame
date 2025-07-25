#pragma once

#include <HandmadeMath.h>

void initPrimitives();
void cleanupPrimitives();

void drawRect(HMM_Vec2 position, HMM_Vec2 size, float rotation, HMM_Vec3 color, int shaderProgram);
void drawSquare(HMM_Vec2 position, float size, float rotation, HMM_Vec3 color, int shaderProgram);
void drawCircle(HMM_Vec2 position, float radius, HMM_Vec3 color, int shaderProgram);
