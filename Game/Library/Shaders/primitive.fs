#version 330 core

in vec3 glColor;

out vec4 Color;

void main()
{
    Color = vec4(glColor, 1.0);
}
