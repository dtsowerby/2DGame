#pragma once

#include <HandmadeMath.h>
#include "gfx/texture.h"
#include "gfx/animation.h"
#include "gfx/particle.h"

typedef struct Entity
{
    unsigned int shaderProgram; 
    //Tilemap* tilemap;
    //Texture texture;
    //unsigned int tileID;
    //int isFlipped;
    HMM_Vec2 position;
    HMM_Vec2 scale;
    HMM_Vec3 colour;
    float rotation; // Rotation (right) in radians
    int isVisible;

    ParticleEmitter particleEmitter;
    
    // Animation component
    //Animation animation;
    //int hasAnimation; // 1 if entity uses animation, 0 if it uses static tileID

    // ENTITY TYPE SPECIFIC

    //Bomb
    float timeAfterBomb;
    HMM_Vec2 bombShadowPosition;

} Entity;

Entity instantiateEntity(Entity* entity);

void updateEntity(Entity* entity);

// Animation helper functions for entities
//void setEntityAnimation(Entity* entity, Animation anim);
//void updateEntityAnimation(Entity* entity, float deltaTime);
//unsigned int getEntityCurrentTileID(Entity* entity);
//void clearEntityAnimation(Entity* entity);
