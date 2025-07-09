#version 330 core

in vec2 TexCoords;

uniform int tileID;

out vec4 fragColour;

uniform sampler2D image;
uniform vec3 tileColour;
uniform int tileDimensions; // Dimensions of a single tile in the tileset
uniform int tilesetSize;    // Dimensions of the entire tileset texture

void main() {
    // Calculate how many tiles fit in one row/column
    int tilesPerRow = tilesetSize / tileDimensions;
    
    // Calculate tile position in the grid
    int tileX = tileID % tilesPerRow;
    int tileY = tileID / tilesPerRow;
    
    // Calculate the UV offset for this tile (top-left corner of the tile)
    vec2 tileOffset = vec2(
        float(tileX * tileDimensions) / float(tilesetSize),
        float(tileY * tileDimensions) / float(tilesetSize)
    );
    
    // Scale the texture coordinates to fit within one tile
    vec2 tileScale = vec2(float(tileDimensions) / float(tilesetSize));
    
    // Calculate final UV coordinates
    vec2 finalUV = tileOffset + TexCoords * tileScale;
    
    // Sample color from the tileset texture
    fragColour = texture(image, finalUV) * vec4(tileColour, 1.0);
}