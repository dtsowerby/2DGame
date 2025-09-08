#version 300 es
precision mediump float;
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aColor;

out vec2 TexCoords;
out vec3 VertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoord;
    VertexColor = aColor;
    gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
}
