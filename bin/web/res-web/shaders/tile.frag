#version 300 es
precision mediump float;

in vec2 TexCoords;

uniform int tileID;

out vec4 fragColour;

uniform sampler2D image;
uniform vec3 tileColour;
uniform int isFlipped; // 0 for normal
uniform int tileDimensions;
uniform int tilesetSize;

void main() {
    int tilesPerRow = tilesetSize / tileDimensions;
    int tileX = tileID % tilesPerRow;
    int tileY = tileID / tilesPerRow;
    vec2 tileOffset = vec2(float(tileX * tileDimensions) / float(tilesetSize), float(tileY * tileDimensions) / float(tilesetSize));
    vec2 tileScale = vec2(float(tileDimensions) / float(tilesetSize));
    vec2 flippedTexCoord = vec2(abs(float(isFlipped) - TexCoords.x), TexCoords.y);
    vec2 finalUV = tileOffset + flippedTexCoord * tileScale;
    fragColour = texture(image, finalUV) * vec4(tileColour, 1.0);
}