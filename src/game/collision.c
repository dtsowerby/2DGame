#include "collision.h"

int checkCCCollision(HMM_Vec2 pos1, float radius1, 
                   HMM_Vec2 pos2, float radius2) {
    HMM_Vec2 diff = HMM_SubV2(pos1, pos2);
    float distanceSquared = HMM_LenSqrV2(diff);
    float radiusSum = radius1 + radius2;
    return (int)(distanceSquared <= (radiusSum * radiusSum));
}

int checkCSCollision(HMM_Vec2 pos1, HMM_Vec2 size1, 
                   HMM_Vec2 pos2, float radius) {
    // Check if the circle's center is within the rectangle's bounds
    if (pos2.X >= pos1.X - radius && pos2.X - radius <= pos1.X + size1.X &&
        pos2.Y >= pos1.Y - radius && pos2.Y - radius <= pos1.Y + size1.Y) {
        return 1; // Collision detected
    }
    return 0;
}