#version 410 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture1;

void main()
{    
    vec4 texColor = texture(texture1, TexCoords);
    color = texColor;
}
