#pragma once

#define STB_IMAGE_IMPLEMENTATION

typedef struct
{
    unsigned int id;
    int width, height, nrChannels;
} Texture;

Texture loadTexture(const char* name);