#include <glad.h>
#include <HandmadeMath.h>

#include "texture.h"
#include "shader.h"
#include "entity.h"
#include "state.h"

#include "sprite_renderer.h"

HMM_Vec2 screenToWorld(HMM_Vec2 position, HMM_Vec2 scale)
{
    return (HMM_Vec2){(position.X*state.windowWidth) - (0.5f * scale.X), (position.Y*state.windowHeight) - (0.5f * scale.Y)};
}

void drawSprite(int shaderProgram, Texture* texture, HMM_Vec2 position, HMM_Vec2 scale, float rotate, HMM_Vec3 colour)
{       
    // prepare transformations
    useShader(shaderProgram);

    HMM_Mat4 projection = HMM_Orthographic_RH_NO(0.0f, (float)state.windowWidth, (float)state.windowHeight, 0.0f, -1.0f, 1.0f);
    setUniformMat4("projection", projection, shaderProgram);

    HMM_Mat4 view = HMM_Translate((HMM_Vec3){-state.camX, -state.camY, 0.0f});

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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    setUniformInt1("image", 0, shaderProgram);

    glBindVertexArray(state.spriteVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void drawEntity(Entity* entity)
{
    drawSprite(entity->shaderProgram, &entity->texture, entity->position, entity->scale, entity->rotation, entity->colour);
}

void drawTile(Tilemap* tilemap, int tileID, HMM_Vec2 position, HMM_Vec3 colour, int shaderProgram)
{   
    useShader(shaderProgram);
    setUniformInt1("tileID", tileID, shaderProgram);
    setUniformInt1("tilesetSize", tilemap->tileWidth * tilemap->tileCountX, shaderProgram);
    setUniformInt1("tileDimensions", tilemap->tileWidth, shaderProgram);
    drawSprite(shaderProgram, &tilemap->texture, position, (HMM_Vec2){(float)state.tileDim, (float)state.tileDim}, 0, colour);
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