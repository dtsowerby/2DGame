#include "../entity.h"
#include <stdio.h>

Entity duplicate(Entity* entity)
{
    Entity newEntity = *entity;
    return newEntity;
}

void updateEntity(Entity* entity)
{
    if (entity == NULL) return;

    // Update particle emitter
    if (entity->particleEmitter.isActive) {
        entity->particleEmitter.position = entity->position;
        updateParticleEmitter(&entity->particleEmitter);
    }
}

// Animation Component - Deprecated

/*void setEntityAnimation(Entity* entity, Animation anim) {
    if (entity == NULL) return;
    
    entity->animation = anim;
    entity->hasAnimation = 1;
    startAnimation(&entity->animation);
}

void updateEntityAnimation(Entity* entity, float deltaTime) {
    if (entity == NULL || !entity->hasAnimation) return;
    
    updateAnimation(&entity->animation, deltaTime);
}

unsigned int getEntityCurrentTileID(Entity* entity) {
    if (entity == NULL) return 0;
    
    if (entity->hasAnimation) {
        return getCurrentFrame(&entity->animation);
    } else {
        return entity->tileID;
    }
}

void clearEntityAnimation(Entity* entity) {
    if (entity == NULL || !entity->hasAnimation) return;
    
    destroyAnimation(&entity->animation);
    entity->hasAnimation = 0;
}*/
