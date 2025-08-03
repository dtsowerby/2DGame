// Kinda cleaner?

void addSegment(EntityList* segmentsList, HMM_Vec3 colour)
{   
    segmentCount++;
    
    // Recalculate all existing segments with the new total count
    for(int i = 0; i < getEntityListSize(segmentsList); i++) {
        Entity* segment = getEntityAtIndex(segmentsList, i);
        if (segment == NULL) continue;

        segment->scale.X = 3.14f/2.0f + i * (6.28f/(float)(segmentCount));
        segment->scale.Y = 6.28f/(float)(segmentCount);

        //printf("Segment %d: X = %.2f, Y = %.2f\n", i, segment->scale.X, segment->scale.Y);
    }
    
    // Add new segment with correct index
    int newSegmentIndex = getEntityListSize(segmentsList);
    pushBack(segmentsList, (Entity){.scale = (HMM_Vec2){3.14f/2.0f + newSegmentIndex * (6.28f/(float)(segmentCount)), 6.28f/(float)(segmentCount)}, .colour = colour});
    //printf("Segment %d: X = %.2f, Y = %.2f\n", newSegmentIndex, 3.14f/2.0f + newSegmentIndex * (6.28f/(float)(segmentCount)), (6.28f/(float)(segmentCount)));
}

void checkWheel()
{   
    if(state.stageCleared == 1 || switchingScene == 1) return;
    wheel_colour = -1.0f;

    for(int i = 0; i < getEntityListSize(segments); i++) {
        Entity* segment = getEntityAtIndex(segments, i);
        if (segment == NULL) continue;
        if(fmod(getMouseAngle() + 3.14f, 6.28f) > segment->scale.X && fmod(getMouseAngle() + 3.14f, 6.28f) < segment->scale.X + segment->scale.Y) {
            wheel_colour = segment->colour.X;
            break;
        }
    }

    if(wheel_colour == last_colour) {
        return;
    }
    last_colour = wheel_colour;

    if(wheel_colour == 0.0f) {
        Entity newBomb = instantiateEntity(&bomb);
        newBomb.position = (HMM_Vec2){player.position.X, player.position.Y};
        newBomb.scale = newBomb.position;
        newBomb.particleEmitter = createBombEmitter((HMM_Vec2){player.position.X, player.position.Y}, 50);
        newBomb.timeAfterInstantiation = state.time;
        newBomb.rotation = getMouseAngle();
        
        pushBack(bombEntities, newBomb);
        startEmitter(&newBomb.particleEmitter);
    } else {
        Entity newProjectile = instantiateEntity(&projectile);
        newProjectile.position = (HMM_Vec2){player.position.X, player.position.Y};
        newProjectile.particleEmitter = createProjectileEmitter((HMM_Vec2){player.position.X, player.position.Y}, 50);
        newProjectile.rotation = getMouseAngle();
        newProjectile.timeAfterInstantiation = state.time;

        if(wheel_colour == 1.0f) {
            setEmitterColorRange(&newProjectile.particleEmitter, (HMM_Vec3){1.0f, 0.0f, 0.0f}, (HMM_Vec3){0.8f, 0.1f, 0.1f});
            newProjectile.projectileType = 1;
        } else {
            setEmitterColorRange(&newProjectile.particleEmitter, (HMM_Vec3){0.0f, 0.0f, 1.0f}, (HMM_Vec3){0.1f, 0.1f, 0.8f});
            newProjectile.projectileType = 0;
        }

        setEmitterScaleRange(&newProjectile.particleEmitter, 0.25f * powerUp * (lastShot/0.08f) * getEntityListSize(segments)/20.0f, 0.5f * powerUp * (lastShot/0.1f) * getEntityListSize(segments)/15.0f);
        newProjectile.health = powerUp * 1.3f * (lastShot/0.08f) * getEntityListSize(segments)/15.0f;

        pushBack(projectileEntities, newProjectile);
        startEmitter(&newProjectile.particleEmitter);
        playSound(shoot);
        lastShot = 0.0f;
    }
}