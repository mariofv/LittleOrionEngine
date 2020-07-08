#version 330 core

in vec2 texCoord;

in vec2 frame;

out vec4 FragColor;

struct Trail {
  vec3 center_pos;
  sampler2D texture;

  vec4 color;

};

void main()
{
    FragColor = vec4(0.0, 1.0, 0.0, 1.0);   
}  