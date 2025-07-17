#include "animation.h"
#include <stdlib.h>
#include <string.h>

Animation createAnimation(unsigned int* frames, unsigned int frameCount, float frameRate, AnimationType type) {
    Animation anim = {0};
    
    // Allocate memory for frames array
    anim.frames = (unsigned int*)malloc(frameCount * sizeof(unsigned int));
    if (anim.frames != NULL) {
        memcpy(anim.frames, frames, frameCount * sizeof(unsigned int));
    }
    
    anim.frameCount = frameCount;
    anim.frameRate = frameRate;
    anim.type = type;
    anim.state = ANIMATION_STOPPED;
    anim.currentTime = 0.0f;
    anim.currentFrame = 0;
    anim.direction = 1;
    
    return anim;
}

void startAnimation(Animation* anim) {
    if (anim == NULL) return;
    
    anim->state = ANIMATION_PLAYING;
    anim->currentTime = 0.0f;
    anim->currentFrame = 0;
    anim->direction = 1;
}

void stopAnimation(Animation* anim) {
    if (anim == NULL) return;
    
    anim->state = ANIMATION_STOPPED;
    anim->currentTime = 0.0f;
    anim->currentFrame = 0;
    anim->direction = 1;
}

void pauseAnimation(Animation* anim) {
    if (anim == NULL) return;
    
    if (anim->state == ANIMATION_PLAYING) {
        anim->state = ANIMATION_PAUSED;
    }
}

void resumeAnimation(Animation* anim) {
    if (anim == NULL) return;
    
    if (anim->state == ANIMATION_PAUSED) {
        anim->state = ANIMATION_PLAYING;
    }
}

void resetAnimation(Animation* anim) {
    if (anim == NULL) return;
    
    anim->currentTime = 0.0f;
    anim->currentFrame = 0;
    anim->direction = 1;
}

void updateAnimation(Animation* anim, float deltaTime) {
    if (anim == NULL || anim->state != ANIMATION_PLAYING || anim->frameCount == 0) {
        return;
    }
    
    anim->currentTime += deltaTime;
    
    float frameDuration = 1.0f / anim->frameRate;
    
    // Check if we need to advance to the next frame
    while (anim->currentTime >= frameDuration) {
        anim->currentTime -= frameDuration;
        
        switch (anim->type) {
            case ANIMATION_LOOP:
                anim->currentFrame = (anim->currentFrame + 1) % anim->frameCount;
                break;
                
            case ANIMATION_ONCE:
                if (anim->currentFrame < anim->frameCount - 1) {
                    anim->currentFrame++;
                } else {
                    anim->state = ANIMATION_STOPPED;
                }
                break;
                
            case ANIMATION_PINGPONG:
                anim->currentFrame += anim->direction;
                
                // Check for direction change
                if (anim->currentFrame >= anim->frameCount - 1) {
                    anim->currentFrame = anim->frameCount - 1;
                    anim->direction = -1;
                } else if (anim->currentFrame <= 0) {
                    anim->currentFrame = 0;
                    anim->direction = 1;
                }
                break;
                
            case ANIMATION_NONE:
            default:
                // No animation, stay on current frame
                break;
        }
    }
}

unsigned int getCurrentFrame(Animation* anim) {
    if (anim == NULL || anim->frames == NULL || anim->frameCount == 0) {
        return 0; // Return default frame
    }
    
    if (anim->currentFrame >= anim->frameCount) {
        return anim->frames[0]; // Safety fallback
    }
    
    return anim->frames[anim->currentFrame];
}

int isAnimationComplete(Animation* anim) {
    if (anim == NULL) return 1;
    
    return (anim->type == ANIMATION_ONCE && anim->state == ANIMATION_STOPPED);
}

Animation createToggleAnimation(unsigned int frame1, unsigned int frame2, float frameRate) {
    unsigned int* frames = (unsigned int*)malloc(2 * sizeof(unsigned int));
    frames[0] = frame1;
    frames[1] = frame2;
    
    return createAnimation(frames, 2, frameRate, ANIMATION_LOOP);
}

// Cleanup function (should be called when animation is no longer needed)
void destroyAnimation(Animation* anim) {
    if (anim != NULL && anim->frames != NULL) {
        free(anim->frames);
        anim->frames = NULL;
        anim->frameCount = 0;
    }
}
