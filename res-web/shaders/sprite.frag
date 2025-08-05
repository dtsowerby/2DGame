#version 300 es
precision mediump float;
in vec2 TexCoords;
out vec4 colour;

uniform sampler2D image;
uniform vec3 spriteColour;

void main()
{    
    colour = vec4(spriteColour, 1.0) * texture(image, TexCoords);
}