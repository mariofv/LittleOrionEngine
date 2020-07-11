#version 330 core

in vec3 col;
in vec2 texCoord;
out vec4 FragColor;

struct Trail {

  vec4 color;

};

void main()
{
    FragColor = vec4(col, 1.0);  	
}  