#pragma once

#include <HandmadeMath.h>

typedef enum ParticleType {
    PARTICLE_STATIC = 0,     // Particles that don't move
    PARTICLE_LINEAR,         // Particles with constant velocity
    PARTICLE_GRAVITY,        // Particles affected by gravity
    PARTICLE_RADIAL,         // Particles moving outward from center
    PARTICLE_ORBIT           // Particles orbiting around a point
} ParticleType;

typedef struct Particle {
    HMM_Vec2 position;       // Current position
    HMM_Vec2 velocity;       // Velocity vector
    HMM_Vec2 acceleration;   // Acceleration vector
    HMM_Vec3 color;          // RGB color (0.0 to 1.0)
    float scale;             // Size scale (1.0 = normal size)
    float rotation;          // Rotation in radians
    float life;              // Current life (0.0 to 1.0, 1.0 = just born)
    float maxLife;           // Maximum life duration in seconds
    int isActive;            // 1 if particle is active, 0 if dead
    
    // Physics properties
    float mass;              // Mass for physics calculations
    float drag;              // Air resistance (0.0 = no drag, 1.0 = high drag)
} Particle;

typedef struct ParticleEmitter {
    HMM_Vec2 position;           // Emitter position
    ParticleType type;           // Type of particles to emit
    
    // Emission properties
    int maxParticles;            // Maximum number of particles
    float emissionRate;          // Particles per second
    float emissionTimer;         // Internal timer for emission
    
    // Particle spawn properties
    HMM_Vec2 velocityMin;        // Minimum initial velocity
    HMM_Vec2 velocityMax;        // Maximum initial velocity
    HMM_Vec3 colorStart;         // Starting color
    HMM_Vec3 colorEnd;           // Ending color (for fade effects)
    float scaleStart;            // Starting scale
    float scaleEnd;              // Ending scale
    float lifeMin;               // Minimum particle life
    float lifeMax;               // Maximum particle life
    
    // Physics properties
    HMM_Vec2 gravity;            // Gravity vector
    float dragMin;               // Minimum drag
    float dragMax;               // Maximum drag
    
    // Spread properties
    float angleSpread;           // Angle spread in radians (for directional emission)
    float baseAngle;             // Base direction angle
    
    // Internal data
    Particle* particles;         // Array of particles
    int activeCount;             // Number of currently active particles
    int isActive;                // 1 if emitter is active, 0 if stopped
} ParticleEmitter;

// Particle system management
ParticleEmitter createParticleEmitter(HMM_Vec2 position, int maxParticles, ParticleType type);
void destroyParticleEmitter(ParticleEmitter* emitter);

// Emitter control
void startEmitter(ParticleEmitter* emitter);
void stopEmitter(ParticleEmitter* emitter);
void pauseEmitter(ParticleEmitter* emitter);
void resumeEmitter(ParticleEmitter* emitter);
void resetEmitter(ParticleEmitter* emitter);

// Update and render
void updateParticleEmitter(ParticleEmitter* emitter, float deltaTime);
void renderParticleEmitter(ParticleEmitter* emitter, unsigned int shaderProgram);

// Configuration helpers
void setEmitterVelocityRange(ParticleEmitter* emitter, HMM_Vec2 minVel, HMM_Vec2 maxVel);
void setEmitterColorRange(ParticleEmitter* emitter, HMM_Vec3 startColor, HMM_Vec3 endColor);
void setEmitterScaleRange(ParticleEmitter* emitter, float startScale, float endScale);
void setEmitterLifeRange(ParticleEmitter* emitter, float minLife, float maxLife);
void setEmitterGravity(ParticleEmitter* emitter, HMM_Vec2 gravity);
void setEmitterSpread(ParticleEmitter* emitter, float baseAngle, float spread);
void setEmitterDragRange(ParticleEmitter* emitter, float minDrag, float maxDrag);

// Preset emitter configurations
ParticleEmitter createExplosionEmitter(HMM_Vec2 position, int particleCount);
ParticleEmitter createSmokeEmitter(HMM_Vec2 position);
ParticleEmitter createSparkEmitter(HMM_Vec2 position);
ParticleEmitter createRainEmitter(HMM_Vec2 position, float width);
ParticleEmitter createFireEmitter(HMM_Vec2 position);
ParticleEmitter createCharacterEmitter(HMM_Vec2 position, int particleCount);
ParticleEmitter createBombEmitter(HMM_Vec2 position, int particleCount);

// Utility functions
void emitParticle(ParticleEmitter* emitter);
void emitBurst(ParticleEmitter* emitter, int count);
int getActiveParticleCount(ParticleEmitter* emitter);
float randomFloat(float min, float max);
HMM_Vec2 randomVec2(HMM_Vec2 min, HMM_Vec2 max);
