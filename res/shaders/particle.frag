#version 330 core

in vec2 TexCoords;
out vec4 fragColour;

uniform vec3 spriteColour;

void main()
{    
    // Simple solid color output for particles
    fragColour = vec4(spriteColour, 1.0);
}
