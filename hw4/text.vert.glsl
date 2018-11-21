#version 410 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(0.0, vertex.y, vertex.x, 1.0); // for main.cpp
    //gl_Position = projection * vec4(vertex.xy, 0.0, 1.0); // for L7.cpp
    TexCoords = vec2(vertex.z, 1.0f - vertex.w);
}
