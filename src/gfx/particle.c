#include "particle.h"
#include "state.h"
#include "sprite_renderer.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// Initialize random seed (call once at startup)
static int randomInitialized = 0;

void initParticleSystem() {
    if (!randomInitialized) {
        srand((unsigned int)time(NULL));
        randomInitialized = 1;
    }
}

float randomFloat(float min, float max) {
    if (!randomInitialized) initParticleSystem();
    return min + (max - min) * ((float)rand() / RAND_MAX);
}

HMM_Vec2 randomVec2(HMM_Vec2 min, HMM_Vec2 max) {
    return (HMM_Vec2){
        randomFloat(min.X, max.X),
        randomFloat(min.Y, max.Y)
    };
}

ParticleEmitter createParticleEmitter(HMM_Vec2 position, int maxParticles, ParticleType type) {
    ParticleEmitter emitter = {0};
    
    emitter.position = position;
    emitter.type = type;
    emitter.maxParticles = maxParticles;
    emitter.emissionRate = 10.0f; // Default 10 particles per second
    emitter.emissionTimer = 0.0f;
    
    // Default values
    emitter.velocityMin = (HMM_Vec2){-1.0f, -1.0f};
    emitter.velocityMax = (HMM_Vec2){1.0f, 1.0f};
    emitter.colorStart = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    emitter.colorEnd = (HMM_Vec3){1.0f, 1.0f, 1.0f};
    emitter.scaleStart = 1.0f;
    emitter.scaleEnd = 0.0f;
    emitter.lifeMin = 1.0f;
    emitter.lifeMax = 2.0f;
    emitter.gravity = (HMM_Vec2){0.0f, 9.81f};
    emitter.dragMin = 0.0f;
    emitter.dragMax = 0.1f;
    emitter.angleSpread = 3.14159f; // Full circle
    emitter.baseAngle = 0.0f;
    
    // Allocate particle array
    emitter.particles = (Particle*)malloc(maxParticles * sizeof(Particle));
    if (emitter.particles) {
        memset(emitter.particles, 0, maxParticles * sizeof(Particle));
    }
    
    emitter.activeCount = 0;
    emitter.isActive = 0;
    
    return emitter;
}

void destroyParticleEmitter(ParticleEmitter* emitter) {
    if (emitter && emitter->particles) {
        free(emitter->particles);
        emitter->particles = NULL;
        emitter->maxParticles = 0;
        emitter->activeCount = 0;
    }
}

void startEmitter(ParticleEmitter* emitter) {
    if (emitter) {
        emitter->isActive = 1;
        emitter->emissionTimer = 0.0f;
    }
}

void stopEmitter(ParticleEmitter* emitter) {
    if (emitter) {
        emitter->isActive = 0;
    }
}

void pauseEmitter(ParticleEmitter* emitter) {
    if (emitter) {
        emitter->isActive = 0;
    }
}

void resumeEmitter(ParticleEmitter* emitter) {
    if (emitter) {
        emitter->isActive = 1;
    }
}

void resetEmitter(ParticleEmitter* emitter) {
    if (emitter && emitter->particles) {
        // Deactivate all particles
        for (int i = 0; i < emitter->maxParticles; i++) {
            emitter->particles[i].isActive = 0;
        }
        emitter->activeCount = 0;
        emitter->emissionTimer = 0.0f;
    }
}

void emitParticle(ParticleEmitter* emitter) {
    if (!emitter || !emitter->particles) return;
    
    // Find an inactive particle
    for (int i = 0; i < emitter->maxParticles; i++) {
        if (!emitter->particles[i].isActive) {
            Particle* p = &emitter->particles[i];
            
            // Initialize particle
            p->position = emitter->position;
            p->velocity = randomVec2(emitter->velocityMin, emitter->velocityMax);
            p->acceleration = (HMM_Vec2){0.0f, 0.0f};
            p->color = emitter->colorStart;
            p->scale = emitter->scaleStart;
            p->rotation = randomFloat(0.0f, 6.28318f); // 0 to 2*PI
            p->maxLife = randomFloat(emitter->lifeMin, emitter->lifeMax);
            p->life = 1.0f; // Start at full life
            p->mass = 1.0f;
            p->drag = randomFloat(emitter->dragMin, emitter->dragMax);
            p->isActive = 1;
            
            // Apply particle type specific modifications
            switch (emitter->type) {
                case PARTICLE_RADIAL:
                    {
                        float angle = emitter->baseAngle + randomFloat(-emitter->angleSpread/2, emitter->angleSpread/2);
                        float speed = HMM_LenV2(p->velocity);
                        p->velocity = (HMM_Vec2){cos(angle) * speed, sin(angle) * speed};
                    }
                    break;
                case PARTICLE_GRAVITY:
                    p->acceleration = emitter->gravity;
                    break;
                case PARTICLE_ORBIT:
                    {
                        float angle = randomFloat(0.0f, 6.28318f);
                        float radius = randomFloat(0.1f, 0.5f);
                        p->position.X += cos(angle) * radius;
                        p->position.Y += sin(angle) * radius;
                        // Orbital velocity perpendicular to radius
                        p->velocity = (HMM_Vec2){-sin(angle), cos(angle)};
                        p->velocity = HMM_MulV2F(p->velocity, randomFloat(0.5f, 2.0f));
                    }
                    break;
                default:
                    break;
            }
            
            emitter->activeCount++;
            break;
        }
    }
}

void emitBurst(ParticleEmitter* emitter, int count) {
    for (int i = 0; i < count; i++) {
        emitParticle(emitter);
    }
}

void updateParticleEmitter(ParticleEmitter* emitter) {
    if (!emitter || !emitter->particles) return;
    
    // Emit new particles if active
    if (emitter->isActive && emitter->emissionRate > 0.0f) {
        emitter->emissionTimer += state.deltaTime;
        float emissionInterval = 1.0f / emitter->emissionRate;
        
        while (emitter->emissionTimer >= emissionInterval) {
            emitParticle(emitter);
            emitter->emissionTimer -= emissionInterval;
        }
    }
    
    // Update existing particles
    emitter->activeCount = 0;
    for (int i = 0; i < emitter->maxParticles; i++) {
        Particle* p = &emitter->particles[i];
        
        if (!p->isActive) continue;
        
        // Update life
        p->life -= state.deltaTime / p->maxLife;
        if (p->life <= 0.0f) {
            p->isActive = 0;
            continue;
        }
        
        emitter->activeCount++;
        
        // Update physics
        p->velocity = HMM_AddV2(p->velocity, HMM_MulV2F(p->acceleration, state.deltaTime));
        
        // Apply drag
        if (p->drag > 0.0f) {
            float dragForce = 1.0f - (p->drag * state.deltaTime);
            if (dragForce < 0.0f) dragForce = 0.0f;
            p->velocity = HMM_MulV2F(p->velocity, dragForce);
        }
        
        // Update position
        p->position = HMM_AddV2(p->position, HMM_MulV2F(p->velocity, state.deltaTime));
        
        // Update visual properties based on life
        float lifeRatio = p->life;
        p->color = HMM_LerpV3(emitter->colorEnd, lifeRatio, emitter->colorStart);
        p->scale = emitter->scaleEnd + (emitter->scaleStart - emitter->scaleEnd) * lifeRatio;
        
        // Handle special particle types
        if (emitter->type == PARTICLE_ORBIT) {
            // Simple orbit around emitter position
            HMM_Vec2 toCenter = HMM_SubV2(emitter->position, p->position);
            float distance = HMM_LenV2(toCenter);
            if (distance > 0.01f) {
                HMM_Vec2 centripetal = HMM_MulV2F(HMM_NormV2(toCenter), 2.0f / distance);
                p->acceleration = centripetal;
            }
        }
    }
}

void renderParticleEmitter(ParticleEmitter* emitter, unsigned int shaderProgram) {
    if (!emitter || !emitter->particles) return;
    
    for (int i = 0; i < emitter->maxParticles; i++) {
        Particle* p = &emitter->particles[i];
        
        if (!p->isActive) continue;
        
        // Calculate scale in screen units
        float screenScale = p->scale * (float)state.tileDim * 0.5f; // Half tile size for particles
        HMM_Vec2 scale = (HMM_Vec2){screenScale, screenScale};
        
        // Render the particle as a colored square using the sprite renderer
        // Pass NULL for texture to render solid color
        drawSprite(shaderProgram, NULL, p->position, scale, p->rotation, p->color);
    }
}

// Configuration helpers
void setEmitterVelocityRange(ParticleEmitter* emitter, HMM_Vec2 minVel, HMM_Vec2 maxVel) {
    if (emitter) {
        emitter->velocityMin = minVel;
        emitter->velocityMax = maxVel;
    }
}

void setEmitterColorRange(ParticleEmitter* emitter, HMM_Vec3 startColor, HMM_Vec3 endColor) {
    if (emitter) {
        emitter->colorStart = startColor;
        emitter->colorEnd = endColor;
    }
}

void setEmitterScaleRange(ParticleEmitter* emitter, float startScale, float endScale) {
    if (emitter) {
        emitter->scaleStart = startScale;
        emitter->scaleEnd = endScale;
    }
}

void setEmitterLifeRange(ParticleEmitter* emitter, float minLife, float maxLife) {
    if (emitter) {
        emitter->lifeMin = minLife;
        emitter->lifeMax = maxLife;
    }
}

void setEmitterGravity(ParticleEmitter* emitter, HMM_Vec2 gravity) {
    if (emitter) {
        emitter->gravity = gravity;
    }
}

void setEmitterSpread(ParticleEmitter* emitter, float baseAngle, float spread) {
    if (emitter) {
        emitter->baseAngle = baseAngle;
        emitter->angleSpread = spread;
    }
}

void setEmitterDragRange(ParticleEmitter* emitter, float minDrag, float maxDrag) {
    if (emitter) {
        emitter->dragMin = minDrag;
        emitter->dragMax = maxDrag;
    }
}

int getActiveParticleCount(ParticleEmitter* emitter) {
    return emitter ? emitter->activeCount : 0;
}

// Preset configurations
ParticleEmitter createExplosionEmitter(HMM_Vec2 position, int particleCount) {
    ParticleEmitter emitter = createParticleEmitter(position, particleCount, PARTICLE_RADIAL);
    emitter.emissionRate = 0.0f; // Don't emit continuously, use burst
    setEmitterVelocityRange(&emitter, (HMM_Vec2){2.0f, 2.0f}, (HMM_Vec2){150.0f, 150.0f});
    setEmitterColorRange(&emitter, (HMM_Vec3){1.0f, 1.0f, 0.0f}, (HMM_Vec3){1.0f, 0.0f, 0.0f}); // Yellow to red
    setEmitterScaleRange(&emitter, 1.5f, 0.0f);
    setEmitterLifeRange(&emitter, 0.25f, 0.75f);
    setEmitterSpread(&emitter, 0.0f, 6.28318f); // Full circle
    setEmitterDragRange(&emitter, 0.5f, 1.0f);
    return emitter;
}

ParticleEmitter createSmokeEmitter(HMM_Vec2 position) {
    ParticleEmitter emitter = createParticleEmitter(position, 50, PARTICLE_LINEAR);
    emitter.emissionRate = 5.0f;
    setEmitterVelocityRange(&emitter, (HMM_Vec2){-0.5f, -2.0f}, (HMM_Vec2){50.0f, -50.0f});
    setEmitterColorRange(&emitter, (HMM_Vec3){0.8f, 0.8f, 0.8f}, (HMM_Vec3){0.2f, 0.2f, 0.2f}); // Light to dark gray
    setEmitterScaleRange(&emitter, 0.5f, 2.0f);
    setEmitterLifeRange(&emitter, 2.0f, 4.0f);
    setEmitterDragRange(&emitter, 0.1f, 0.3f);
    return emitter;
}

ParticleEmitter createSparkEmitter(HMM_Vec2 position) {
    ParticleEmitter emitter = createParticleEmitter(position, 30, PARTICLE_GRAVITY);
    emitter.emissionRate = 15.0f;
    setEmitterVelocityRange(&emitter, (HMM_Vec2){-3.0f, -3.0f}, (HMM_Vec2){300.0f, -100.0f});
    setEmitterColorRange(&emitter, (HMM_Vec3){1.0f, 1.0f, 0.0f}, (HMM_Vec3){1.0f, 0.5f, 0.0f}); // Yellow to orange
    setEmitterScaleRange(&emitter, 0.8f, 0.1f);
    setEmitterLifeRange(&emitter, 0.3f, 0.8f);
    setEmitterGravity(&emitter, (HMM_Vec2){0.0f, 5.0f});
    setEmitterDragRange(&emitter, 0.2f, 0.4f);
    return emitter;
}

ParticleEmitter createRainEmitter(HMM_Vec2 position, float width) {
    ParticleEmitter emitter = createParticleEmitter(position, 100, PARTICLE_LINEAR);
    emitter.emissionRate = 50.0f;
    setEmitterVelocityRange(&emitter, (HMM_Vec2){-0.5f, 3.0f}, (HMM_Vec2){50.0f, 50.0f});
    setEmitterColorRange(&emitter, (HMM_Vec3){0.5f, 0.5f, 1.0f}, (HMM_Vec3){0.3f, 0.3f, 0.8f}); // Light to dark blue
    setEmitterScaleRange(&emitter, 0.3f, 0.1f);
    setEmitterLifeRange(&emitter, 1.0f, 2.0f);
    // Spread rain across width
    emitter.velocityMin.X = -width/2.0f;
    emitter.velocityMax.X = width/2.0f;
    return emitter;
}

ParticleEmitter createFireEmitter(HMM_Vec2 position) {
    ParticleEmitter emitter = createParticleEmitter(position, 40, PARTICLE_LINEAR);
    emitter.emissionRate = 20.0f;
    setEmitterVelocityRange(&emitter, (HMM_Vec2){-1.0f, -3.0f}, (HMM_Vec2){100.0f, -100.0f});
    setEmitterColorRange(&emitter, (HMM_Vec3){1.0f, 0.3f, 0.0f}, (HMM_Vec3){1.0f, 1.0f, 0.0f}); // Red to yellow
    setEmitterScaleRange(&emitter, 1.0f, 0.2f);
    setEmitterLifeRange(&emitter, 0.8f, 1.5f);
    setEmitterDragRange(&emitter, 0.1f, 0.2f);
    return emitter;
}

ParticleEmitter createCharacterEmitter(HMM_Vec2 position, int particleCount) {
    ParticleEmitter emitter = createParticleEmitter(position, particleCount, PARTICLE_RADIAL);
    emitter.emissionRate = 50.0f;
    setEmitterVelocityRange(&emitter, (HMM_Vec2){2.0f, 2.0f}, (HMM_Vec2){50.0f, 50.0f});
    setEmitterColorRange(&emitter, (HMM_Vec3){0.05f, 0.05f, 0.05f}, (HMM_Vec3){0.1f, 0.1f, 0.1f}); // White to black
    setEmitterScaleRange(&emitter, 1.5f, .5f);
    setEmitterLifeRange(&emitter, 0.25f, 0.25f);
    setEmitterSpread(&emitter, 0.0f, 6.28318f); // Full circle
    setEmitterDragRange(&emitter, 0.5f, 1.0f);
    return emitter;
}

ParticleEmitter createEnemyEmitter(HMM_Vec2 position, int particleCount) {
    ParticleEmitter emitter = createParticleEmitter(position, particleCount, PARTICLE_RADIAL);
    emitter.emissionRate = 50.0f;
    setEmitterVelocityRange(&emitter, (HMM_Vec2){2.0f, 2.0f}, (HMM_Vec2){50.0f, 50.0f});
    setEmitterColorRange(&emitter, (HMM_Vec3){0.70f, 0.05f, 0.05f}, (HMM_Vec3){0.1f, 0.1f, 0.1f}); // White to black
    setEmitterScaleRange(&emitter, 1.5f, .5f);
    setEmitterLifeRange(&emitter, 0.25f, 0.25f);
    setEmitterSpread(&emitter, 0.0f, 6.28318f); // Full circle
    setEmitterDragRange(&emitter, 0.5f, 1.0f);
    return emitter;
}

ParticleEmitter createBombEmitter(HMM_Vec2 position, int particleCount) {
    ParticleEmitter emitter = createParticleEmitter(position, particleCount, PARTICLE_RADIAL);
    emitter.emissionRate = 50.0f; // Don't emit continuously, use burst
    setEmitterVelocityRange(&emitter, (HMM_Vec2){2.0f, 2.0f}, (HMM_Vec2){50.0f, 50.0f});
    setEmitterColorRange(&emitter, (HMM_Vec3){0.00f, 0.00f, 0.00f}, (HMM_Vec3){0.1f, 0.1f, 0.1f}); // White to black
    setEmitterScaleRange(&emitter, 0.25f, 0.5f);
    setEmitterLifeRange(&emitter, 0.05f, 0.10f);
    setEmitterSpread(&emitter, 0.0f, 6.28318f); // Full circle
    setEmitterDragRange(&emitter, 0.5f, 1.0f);
    return emitter;
}

