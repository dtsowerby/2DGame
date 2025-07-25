#include <glad.h>
#include <HandmadeMath.h>

#include "texture.h"
#include "shader.h"
#include "entity.h"
#include "state.h"

#include "sprite_renderer.h"

HMM_Vec2 screenToWorld(HMM_Vec2 position, HMM_Vec2 scale)
{   
    // Scale in pixels??
    // function used to actually do something
    return (HMM_Vec2){(position.X) - (0.5f * scale.X), (position.Y) - (0.5f * scale.Y)};
}

// Get the current projection dimensions used for rendering
HMM_Vec2 getProjectionDimensions()
{
    float currentAspect = (float)state.windowWidth / (float)state.windowHeight;
    float referenceAspect = (float)state.referenceWidth / (float)state.referenceHeight;

    float projWidth, projHeight;
    if (currentAspect > referenceAspect) {
        // Window is wider than reference - maintain height, expand width
        projHeight = (float)state.referenceHeight;
        projWidth = (float)state.referenceHeight * currentAspect;
    } else {
        // Window is taller than reference - maintain width, expand height
        projWidth = (float)state.referenceWidth;
        projHeight = (float)state.referenceWidth / currentAspect;
    }
    
    return (HMM_Vec2){projWidth, projHeight};
}

// Convert mouse coordinates to game world coordinates
HMM_Vec2 mouseToWorld(double mouseX, double mouseY)
{
    HMM_Vec2 projDims = getProjectionDimensions();
    
    // Convert mouse coordinates from window space to projection space
    float normalizedX = (float)mouseX / (float)state.windowWidth;
    float normalizedY = (float)mouseY / (float)state.windowHeight;
    
    float worldX = normalizedX * projDims.X;
    float worldY = normalizedY * projDims.Y;
    
    return (HMM_Vec2){worldX, worldY};
}

void drawSprite(int shaderProgram, Texture* texture, HMM_Vec2 position, HMM_Vec2 scale, float rotate, HMM_Vec3 colour)
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

    HMM_Vec2 worldPos = screenToWorld(position, scale);
    model = HMM_MulM4(model, HMM_Translate((HMM_Vec3){worldPos.X, worldPos.Y, 0.0f}));

    model = HMM_MulM4(model, HMM_Translate((HMM_Vec3){0.5f * scale.X, 0.5f * scale.Y, 0.0f}));
    model = HMM_MulM4(model, HMM_Rotate_RH(rotate, (HMM_Vec3){0.0f, 0.0f, 1.0f}));
    model = HMM_MulM4(model, HMM_Translate((HMM_Vec3){-0.5f * scale.X, -0.5f * scale.Y, 0.0f}));

    model = HMM_MulM4(model, HMM_Scale((HMM_Vec3){scale.X, scale.Y, 1.0f}));

    setUniformMat4("model", model, shaderProgram);

    // render textured quad
    setUniformVec3("spriteColour", colour, shaderProgram);

    if (texture != NULL) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->id);
        setUniformInt1("image", 0, shaderProgram);
    } else {
        // For particles without texture, bind a white texture or disable texturing
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
        setUniformInt1("image", 0, shaderProgram);
    }

    glBindVertexArray(state.spriteVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void drawEntity(Entity* entity)
{
    //drawSprite(entity->shaderProgram, &entity->texture, entity->position, entity->scale, entity->rotation, entity->colour);
    //unsigned int currentTileID = getEntityCurrentTileID(entity);
    //drawTile(entity->tilemap, currentTileID, entity->position, entity->colour, entity->isFlipped, entity->shaderProgram);
    entity->particleEmitter.isActive = entity->isVisible;
    renderParticleEmitter(&entity->particleEmitter, entity->shaderProgram);
}

void drawTile(Tilemap* tilemap, int tileID, HMM_Vec2 position, HMM_Vec3 colour, int isFlipped, int shaderProgram)
{   
    useShader(shaderProgram);
    setUniformInt1("tileID", tileID, shaderProgram);
    setUniformInt1("tilesetSize", tilemap->tileWidth * tilemap->tileCountX, shaderProgram);
    setUniformInt1("isFlipped", isFlipped, shaderProgram);
    setUniformInt1("tileDimensions", tilemap->tileWidth, shaderProgram);
    setUniformVec3("tileColour", colour, shaderProgram);
    drawSprite(shaderProgram, &tilemap->texture, position, (HMM_Vec2){(float)state.tileDim * 1.2f, (float)state.tileDim * 1.2f}, 0, colour);
}

void initSpriteRenderer()
{
    // configure VAO/VBO
    float vertices[] = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 
    
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &state.spriteVAO);
    glGenBuffers(1, &state.spriteVBO);

    glBindBuffer(GL_ARRAY_BUFFER, state.spriteVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(state.spriteVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}