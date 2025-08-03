#include <glad.h>
#include <HandmadeMath.h>
#include <math.h>
#include <stdlib.h>

#include "primitive.h"
#include "sprite_renderer.h"
#include "shader.h"
#include "state.h"

// Static variables for primitive rendering
static unsigned int circleVAO = 0;
static unsigned int circleVBO = 0;
static unsigned int circleEBO = 0;
static int maxCircleSegments = 64;

void initPrimitives()
{
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);
    glGenBuffers(1, &circleEBO);
}

void cleanupPrimitives()
{
    if (circleVAO != 0) {
        glDeleteVertexArrays(1, &circleVAO);
        circleVAO = 0;
    }
    if (circleVBO != 0) {
        glDeleteBuffers(1, &circleVBO);
        circleVBO = 0;
    }
    if (circleEBO != 0) {
        glDeleteBuffers(1, &circleEBO);
        circleEBO = 0;
    }
}

void drawRect(HMM_Vec2 position, HMM_Vec2 size, float rotation, HMM_Vec3 colour, int shaderProgram)
{
    // Use the existing sprite renderer to draw a rectangle without a texture
    drawSprite(shaderProgram, NULL, position, size, rotation, colour);
}

void drawBackground(HMM_Vec2 position, float rotation, int shaderProgram)
{
    // prepare transformations
    useShader(shaderProgram);

    // Use a fixed reference resolution to maintain consistent view
    HMM_Vec2 projDims = getProjectionDimensions();
    HMM_Mat4 projection = HMM_Orthographic_RH_NO(0.0f, projDims.X, projDims.Y, 0.0f, -1.0f, 1.0f);
    setUniformMat4("projection", projection, shaderProgram);

    HMM_Mat4 view = HMM_Translate((HMM_Vec3){(float)-state.camX, (float)-state.camY, 0.0f});

    setUniformMat4("view", view, shaderProgram);

    HMM_Mat4 model = HMM_M4D(1.0f);

    HMM_Vec2 size = (HMM_Vec2){projDims.X*2.0f, projDims.Y*2.0f};
    HMM_Vec2 worldPos = screenToWorld(position, size);
    
    model = HMM_MulM4(model, HMM_Translate((HMM_Vec3){worldPos.X, worldPos.Y, 0.0f}));
    model = HMM_MulM4(model, HMM_Translate((HMM_Vec3){size.X * 0.5f, size.Y * 0.5f, 0.0f}));
    model = HMM_MulM4(model, HMM_Rotate_RH(rotation, (HMM_Vec3){0.0f, 0.0f, 1.0f}));
    model = HMM_MulM4(model, HMM_Translate((HMM_Vec3){-size.X * 0.5f, -size.Y * 0.5f, 0.0f}));
    model = HMM_MulM4(model, HMM_Scale((HMM_Vec3){size.X, size.Y, 1.0f}));

    setUniformMat4("model", model, shaderProgram);
    setUniformFloat("time", state.time, shaderProgram);


    glBindVertexArray(state.spriteVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void drawSquare(HMM_Vec2 position, float size, float rotation, HMM_Vec3 colour, int shaderProgram)
{
    HMM_Vec2 squareSize = {size, size};
    drawRect(position, squareSize, rotation, colour, shaderProgram);
}

void drawRectPivot(HMM_Vec2 position, HMM_Vec2 size, float rotation, HMM_Vec2 pivot, HMM_Vec3 colour, int shaderProgram)
{
    useShader(shaderProgram);

    HMM_Vec2 projDims = getProjectionDimensions();
    HMM_Mat4 projection = HMM_Orthographic_RH_NO(0.0f, projDims.X, projDims.Y, 0.0f, -1.0f, 1.0f);
    setUniformMat4("projection", projection, shaderProgram);

    HMM_Mat4 view = HMM_Translate((HMM_Vec3){(float)-state.camX, (float)-state.camY, 0.0f});

    setUniformMat4("view", view, shaderProgram);

    HMM_Mat4 model = HMM_M4D(1.0f);

    HMM_Vec2 worldPos = screenToWorld(position, size);
    model = HMM_MulM4(model, HMM_Translate((HMM_Vec3){worldPos.X, worldPos.Y, 0.0f}));

    model = HMM_MulM4(model, HMM_Translate((HMM_Vec3){0.5f * size.X, 0.5f * size.Y, 0.0f}));
    model = HMM_MulM4(model, HMM_Translate((HMM_Vec3){pivot.X, pivot.Y, 0.0f}));
    model = HMM_MulM4(model, HMM_Rotate_RH(rotation, (HMM_Vec3){0.0f, 0.0f, 1.0f}));
    model = HMM_MulM4(model, HMM_Translate((HMM_Vec3){-pivot.X, -pivot.Y, 0.0f}));
    model = HMM_MulM4(model, HMM_Translate((HMM_Vec3){-0.5f * size.X, -0.5f * size.Y, 0.0f}));

    model = HMM_MulM4(model, HMM_Scale((HMM_Vec3){size.X, size.Y, 1.0f}));

    setUniformMat4("model", model, shaderProgram);

    // render textured quad
    setUniformVec3("spriteColour", colour, shaderProgram);

    glBindVertexArray(state.spriteVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void drawCircle(HMM_Vec2 position, float radius, HMM_Vec3 color, int shaderProgram)
{   
    int segments = 32;
    if (segments < 3) segments = 3;
    if (segments > maxCircleSegments) segments = maxCircleSegments;
    
    // Calculate vertices for a triangle fan circle
    int vertexCount = segments + 2; // center vertex + circle vertices + closing vertex
    float* vertices = (float*)malloc(vertexCount * 4 * sizeof(float)); // 4 floats per vertex (x, y, u, v)
    
    if (!vertices) return; // Memory allocation failed
    
    // Center vertex
    vertices[0] = 0.0f; // x
    vertices[1] = 0.0f; // y
    vertices[2] = 0.5f; // u (texture coordinate)
    vertices[3] = 0.5f; // v (texture coordinate)
    
    // Circle vertices
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.14f * i / segments;
        float x = cosf(angle);
        float y = sinf(angle);
        
        int idx = (i + 1) * 4;
        vertices[idx + 0] = x; // x
        vertices[idx + 1] = y; // y
        vertices[idx + 2] = (x + 1.0f) * 0.5f; // u
        vertices[idx + 3] = (y + 1.0f) * 0.5f; // v
    }
    
    // Setup OpenGL state
    useShader(shaderProgram);

    HMM_Vec2 projDims = getProjectionDimensions();

    // Set up projection and view matrices
    HMM_Mat4 projection = HMM_Orthographic_RH_NO(0.0f, projDims.X, projDims.Y, 0.0f, -1.0f, 1.0f);
    setUniformMat4("projection", projection, shaderProgram);
    
    HMM_Mat4 view = HMM_Translate((HMM_Vec3){(float)-state.camX, (float)-state.camY, 0.0f});
    setUniformMat4("view", view, shaderProgram);
    
    // Set up model matrix
    HMM_Mat4 model = HMM_M4D(1.0f);
    HMM_Vec2 scale = {radius * 2.0f, radius * 2.0f};
    HMM_Vec2 worldPos = screenToWorld(position, scale);
    
    model = HMM_MulM4(model, HMM_Translate((HMM_Vec3){worldPos.X, worldPos.Y, 0.0f}));
    model = HMM_MulM4(model, HMM_Translate((HMM_Vec3){radius, radius, 0.0f}));
    model = HMM_MulM4(model, HMM_Scale((HMM_Vec3){radius, radius, 1.0f}));
    
    setUniformMat4("model", model, shaderProgram);
    setUniformVec3("spriteColour", color, shaderProgram);
    
    // Disable texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    setUniformInt1("image", 0, shaderProgram);
    
    // Upload vertex data
    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 4 * sizeof(float), vertices, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    // Draw triangle fan
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
    
    // Cleanup
    glBindVertexArray(0);
    free(vertices);
}

void drawCircleSegment(HMM_Vec2 position, float radius, float rotation, float arcLength, HMM_Vec3 color, int shaderProgram)
{   
    int segments = 32;
    if (segments < 3) segments = 3;
    if (segments > maxCircleSegments) segments = maxCircleSegments;
    
    // Calculate vertices for a triangle fan circle
    int vertexCount = segments + 2; // center vertex + circle vertices + closing vertex
    float* vertices = (float*)malloc(vertexCount * 4 * sizeof(float)); // 4 floats per vertex (x, y, u, v)
    
    if (!vertices) return; // Memory allocation failed
    
    // Center vertex
    vertices[0] = 0.0f; // x
    vertices[1] = 0.0f; // y
    vertices[2] = 0.5f; // u (texture coordinate)
    vertices[3] = 0.5f; // v (texture coordinate)
    
    // Circle vertices
    for (int i = 0; i <= segments; i++) {
        float angle = arcLength * i / segments;
        float x = cosf(angle);
        float y = sinf(angle);
        
        int idx = (i + 1) * 4;
        vertices[idx + 0] = x; // x
        vertices[idx + 1] = y; // y
        vertices[idx + 2] = (x + 1.0f) * 0.5f; // u
        vertices[idx + 3] = (y + 1.0f) * 0.5f; // v
    }
    
    // Setup OpenGL state
    useShader(shaderProgram);

    HMM_Vec2 projDims = getProjectionDimensions();

    // Set up projection and view matrices
    HMM_Mat4 projection = HMM_Orthographic_RH_NO(0.0f, projDims.X, projDims.Y, 0.0f, -1.0f, 1.0f);
    setUniformMat4("projection", projection, shaderProgram);
    
    HMM_Mat4 view = HMM_Translate((HMM_Vec3){(float)-state.camX, (float)-state.camY, 0.0f});
    setUniformMat4("view", view, shaderProgram);
    
    // Set up model matrix
    HMM_Mat4 model = HMM_M4D(1.0f);
    HMM_Vec2 scale = {radius * 2.0f, radius * 2.0f};
    HMM_Vec2 worldPos = screenToWorld(position, scale);
    
    model = HMM_MulM4(model, HMM_Translate((HMM_Vec3){worldPos.X, worldPos.Y, 0.0f}));
    model = HMM_MulM4(model, HMM_Translate((HMM_Vec3){radius, radius, 0.0f}));
    model = HMM_MulM4(model, HMM_Rotate_RH(rotation, (HMM_Vec3){0.0f, 0.0f, 1.0f}));
    model = HMM_MulM4(model, HMM_Scale((HMM_Vec3){radius, radius, 1.0f}));
    
    setUniformMat4("model", model, shaderProgram);
    setUniformVec3("spriteColour", color, shaderProgram);
    
    // Disable texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    setUniformInt1("image", 0, shaderProgram);
    
    // Upload vertex data
    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 4 * sizeof(float), vertices, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    // Draw triangle fan
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
    
    // Cleanup
    glBindVertexArray(0);
    free(vertices);
}
