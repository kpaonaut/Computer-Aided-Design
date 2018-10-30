#version 410 core

out vec4 color;
uniform vec4 varyingColor;

void main()
{
    color = varyingColor;
}