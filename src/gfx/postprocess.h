#pragma once

#include <glad.h>
#include <HandmadeMath.h>

typedef struct PostProcessor {
    unsigned int framebuffer;
    unsigned int colorTexture;
    unsigned int depthBuffer;
    unsigned int shaderProgram;
    unsigned int quadVAO;
    unsigned int quadVBO;
    
    // Effect parameters
    float vignetteStrength;
    float chromaticAberrationStrength;
    
    // Screen dimensions
    int width;
    int height;
} PostProcessor;

// Function declarations
void initPostProcessor(PostProcessor* processor, int width, int height);
void beginPostProcessing(PostProcessor* processor);
void endPostProcessing(PostProcessor* processor);
void renderPostProcessed(PostProcessor* processor, float time);
void resizePostProcessor(PostProcessor* processor, int width, int height);
void cleanupPostProcessor(PostProcessor* processor);
void setPostProcessingParams(PostProcessor* processor, float vignette, float chromatic);
