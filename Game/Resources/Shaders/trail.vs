#version 330 core
layout (location = 0) in vec3 vertex; 

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
	mat4 view;
} matrices;

vec4 color;
out vec3 col;

void main()
{
    gl_Position = matrices.proj * matrices.view * vec4(vertex, 1.0f);
	col = vertex;
}  