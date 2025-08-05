#include "texture.h"
#include "stb_image.h"
#include "glad.h"

Texture loadTexture(const char* name)
{
    int width, height, nrChannels;
    unsigned char *data = stbi_load(name, &width, &height, &nrChannels, 0); 

    unsigned int texID;
    glGenTextures(1, &texID);  

    glBindTexture(GL_TEXTURE_2D, texID);

    GLfloat borderColor[] = {0.0f, 0.0f, 0.0f, 0.0f}; 
    #ifndef __EMSCRIPTEN__
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    #endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else 
    {
        printf("Texture failed to load: %s\n", name);
    }

    stbi_image_free(data);

    Texture texture;
    texture.id = texID;
    texture.width = width;
    texture.height = height;
    texture.nrChannels = nrChannels;

    return texture;
}

Tilemap createTilemap(Texture texture, int tileWidth, int tileHeight, int tileCountX, int tileCountY)
{
    Tilemap tilemap;
    tilemap.texture = texture;
    tilemap.tileWidth = tileWidth;
    tilemap.tileCountX = tileCountX;

    return tilemap;
}