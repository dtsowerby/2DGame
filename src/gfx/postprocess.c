#include <glad.h>
#include <stdio.h>
#include <HandmadeMath.h>

#include "postprocess.h"
#include "shader.h"
#include "state.h"

void initPostProcessor(PostProcessor* processor, int width, int height)
{
    processor->width = width;
    processor->height = height;
    
    // Default effect parameters
    processor->vignetteStrength = 0.8f;
    processor->chromaticAberrationStrength = 0.002f;
    
    // Load post-processing shader
    processor->shaderProgram = createShaderProgramS("res/shaders/postprocess.vert", "res/shaders/postprocess.frag");
    
    // Create framebuffer
    glGenFramebuffers(1, &processor->framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, processor->framebuffer);
    
    // Create color texture
    glGenTextures(1, &processor->colorTexture);
    glBindTexture(GL_TEXTURE_2D, processor->colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, processor->colorTexture, 0);
    
    // Create depth buffer
    glGenRenderbuffers(1, &processor->depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, processor->depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, processor->depthBuffer);
    
    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("ERROR: Framebuffer not complete!\n");
    }
    
    // Create quad for rendering
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    glGenVertexArrays(1, &processor->quadVAO);
    glGenBuffers(1, &processor->quadVBO);
    glBindVertexArray(processor->quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, processor->quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    // Texture coordinate attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void beginPostProcessing(PostProcessor* processor)
{
    // Bind our framebuffer and set viewport to framebuffer size
    glBindFramebuffer(GL_FRAMEBUFFER, processor->framebuffer);
    glViewport(0, 0, processor->width, processor->height);
}

void endPostProcessing(PostProcessor* processor)
{   
    (void)processor;
    // Bind back to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Reset viewport to window size
    glViewport(0, 0, state.windowWidth, state.windowHeight);
}

void renderPostProcessed(PostProcessor* processor, float time)
{
    // Disable depth testing for screen quad
    glDisable(GL_DEPTH_TEST);
    
    // Use post-processing shader
    useShader(processor->shaderProgram);
    
    // Set uniforms
    setUniformFloat("time", time, processor->shaderProgram);
    setUniformVec2("resolution", (HMM_Vec2){(float)processor->width, (float)processor->height}, processor->shaderProgram);
    setUniformFloat("vignetteStrength", processor->vignetteStrength, processor->shaderProgram);
    setUniformFloat("chromaticAberrationStrength", processor->chromaticAberrationStrength, processor->shaderProgram);
    
    // Bind the color texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, processor->colorTexture);
    setUniformInt1("screenTexture", 0, processor->shaderProgram);
    
    // Render the quad
    glBindVertexArray(processor->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
}

void resizePostProcessor(PostProcessor* processor, int width, int height)
{
    processor->width = width;
    processor->height = height;
    
    // Resize color texture
    glBindTexture(GL_TEXTURE_2D, processor->colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    
    // Resize depth buffer
    glBindRenderbuffer(GL_RENDERBUFFER, processor->depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}

void setPostProcessingParams(PostProcessor* processor, float vignette, float chromatic)
{
    processor->vignetteStrength = vignette;
    processor->chromaticAberrationStrength = chromatic;
}

void cleanupPostProcessor(PostProcessor* processor)
{
    glDeleteFramebuffers(1, &processor->framebuffer);
    glDeleteTextures(1, &processor->colorTexture);
    glDeleteRenderbuffers(1, &processor->depthBuffer);
    glDeleteVertexArrays(1, &processor->quadVAO);
    glDeleteBuffers(1, &processor->quadVBO);
}
