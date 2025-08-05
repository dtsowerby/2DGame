#version 300 es
precision mediump float;
in vec2 TexCoords;
in vec3 VertexColor;
out vec4 FragColor;

uniform sampler2D fontTexture;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(fontTexture, TexCoords).r);
    FragColor = vec4(VertexColor, 1.0) * sampled;
}
