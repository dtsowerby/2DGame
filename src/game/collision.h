#pragma once

#include <HandmadeMath.h>

// Circle-Circle collision
int checkCCCollision(HMM_Vec2 pos1, float radius1, 
                   HMM_Vec2 pos2, float radius2);

// Circle-Square/Rectangle collision
int checkCSCollision(HMM_Vec2 pos1, HMM_Vec2 size1, 
                   HMM_Vec2 pos2, float radius);

// Rectangle-Rectangle collision (AABB)
int checkRRCollision(HMM_Vec2 pos1, HMM_Vec2 size1,
                   HMM_Vec2 pos2, HMM_Vec2 size2);

// Point-Circle collision
int checkPCCollision(HMM_Vec2 point, HMM_Vec2 circlePos, float radius);

// Point-Rectangle collision
int checkPRCollision(HMM_Vec2 point, HMM_Vec2 rectPos, HMM_Vec2 rectSize);

