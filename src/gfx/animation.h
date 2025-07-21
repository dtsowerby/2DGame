#pragma once

typedef enum AnimationType {
    ANIMATION_NONE = 0,
    ANIMATION_LOOP,      // Loop continuously 
    ANIMATION_ONCE,      // Play once then stop
    ANIMATION_PINGPONG   // Play forward then backward, repeat
} AnimationType;

typedef enum AnimationState {
    ANIMATION_STOPPED = 0,
    ANIMATION_PLAYING,
    ANIMATION_PAUSED
} AnimationState;

typedef struct Animation {
    unsigned int* frames;        // Array of tile IDs for animation frames
    unsigned int frameCount;     // Number of frames in the animation
    float frameRate;            // Frames per second (e.g., 5.0f for 5 FPS)
    AnimationType type;         // How the animation should behave
    AnimationState state;       // Current state of the animation
    
    // Internal timing variables
    float currentTime;          // Current time within the animation
    unsigned int currentFrame;  // Current frame index
    int direction;              // 1 for forward, -1 for backward (used in PINGPONG)
} Animation;

// Animation management functions
Animation createAnimation(unsigned int* frames, unsigned int frameCount, float frameRate, AnimationType type);
void startAnimation(Animation* anim);
void stopAnimation(Animation* anim);
void pauseAnimation(Animation* anim);
void resumeAnimation(Animation* anim);
void resetAnimation(Animation* anim);

// Update animation (call this every frame)
void updateAnimation(Animation* anim, float deltaTime);

// Get current frame tile ID
unsigned int getCurrentFrame(Animation* anim);

// Check if animation is complete (useful for ANIMATION_ONCE)
int isAnimationComplete(Animation* anim);

// Utility function to create simple 2-frame toggle animations (like your current pt system)
Animation createToggleAnimation(unsigned int frame1, unsigned int frame2, float frameRate);

// Cleanup function (call when animation is no longer needed)
void destroyAnimation(Animation* anim);
