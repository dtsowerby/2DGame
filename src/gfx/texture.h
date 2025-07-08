#pragma once

#define STB_IMAGE_IMPLEMENTATION

typedef struct
{
    unsigned int id;
    int width, height, nrChannels;
} Texture;

typedef struct 
{
    Texture texture;
    int tileWidth;
    int tileCountX;
} Tilemap;

Texture loadTexture(const char* name);
Tilemap createTilemap(Texture texture, int tileWidth, int tileHeight, int tileCountX, int tileCountY);

