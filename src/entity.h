#pragma once

#include <HandmadeMath.h>
#include "gfx/texture.h"

typedef struct Entity
{
    unsigned int shaderProgram; 
    Texture texture;
    HMM_Vec2 position;
    HMM_Vec2 scale;
    HMM_Vec3 colour;
    float rotation; // Rotation in radians
} Entity;
