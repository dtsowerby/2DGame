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
    // Find the closest point on the rectangle to the circle center
    float closestX = HMM_MAX(pos1.X, HMM_MIN(pos2.X, pos1.X + size1.X));
    float closestY = HMM_MAX(pos1.Y, HMM_MIN(pos2.Y, pos1.Y + size1.Y));
    
    // Calculate the distance between the circle center and closest point
    float deltaX = pos2.X - closestX;
    float deltaY = pos2.Y - closestY;
    float distanceSquared = (deltaX * deltaX) + (deltaY * deltaY);
    
    // Check if the distance is less than or equal to the circle's radius
    return (int)(distanceSquared <= (radius * radius));
}

// Rectangle-Rectangle collision (AABB - Axis-Aligned Bounding Box)
int checkRRCollision(HMM_Vec2 pos1, HMM_Vec2 size1,
                   HMM_Vec2 pos2, HMM_Vec2 size2) {
    return (int)(pos1.X < pos2.X + size2.X && 
                 pos1.X + size1.X > pos2.X && 
                 pos1.Y < pos2.Y + size2.Y && 
                 pos1.Y + size1.Y > pos2.Y);
}

// Point-Circle collision
int checkPCCollision(HMM_Vec2 point, HMM_Vec2 circlePos, float radius) {
    HMM_Vec2 diff = HMM_SubV2(point, circlePos);
    float distanceSquared = HMM_LenSqrV2(diff);
    return (int)(distanceSquared <= (radius * radius));
}

// Point-Rectangle collision
int checkPRCollision(HMM_Vec2 point, HMM_Vec2 rectPos, HMM_Vec2 rectSize) {
    return (int)(point.X >= rectPos.X && 
                 point.X <= rectPos.X + rectSize.X && 
                 point.Y >= rectPos.Y && 
                 point.Y <= rectPos.Y + rectSize.Y);
}