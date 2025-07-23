#pragma once

#include <HandmadeMath.h>

int checkCCCollision(HMM_Vec2 pos1, float radius1, 
                   HMM_Vec2 pos2, float radius2);

int checkCSCollision(HMM_Vec2 pos1, HMM_Vec2 size1, 
                   HMM_Vec2 pos2, float radius);

