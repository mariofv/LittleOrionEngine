#version 330 core

uniform float u_time;

out vec4 color;

void main()
{
 color = vec4(1 - sin(u_time/128), 1.0, sin(u_time/128), 1.0);
}
