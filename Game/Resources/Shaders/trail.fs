#version 330 core

in vec3 col;
out vec4 FragColor;

struct Trail {

  vec4 color;

};

void main()
{
    FragColor = vec4(col, 1.0);   
}  