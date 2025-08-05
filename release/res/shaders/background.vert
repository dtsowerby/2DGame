#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 fragCoords;

void main()
{
    vec4 worldPos = model * vec4(vertex.xy, 0.0, 1.0);
    gl_Position = projection * view * worldPos;
    fragCoords = worldPos.xy;
}
