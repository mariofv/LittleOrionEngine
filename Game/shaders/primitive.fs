#version 330 core

in vec3 glColor;

out vec4 color;

void main()
{
    color = vec4(glColor, 1.0);
}
