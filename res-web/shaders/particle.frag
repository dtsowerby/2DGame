#version 300 es
precision mediump float;

in vec2 TexCoords;
out vec4 fragColour;

uniform vec3 spriteColour;

void main()
{    
    // colour
    fragColour = vec4(spriteColour, 1.0);
}
