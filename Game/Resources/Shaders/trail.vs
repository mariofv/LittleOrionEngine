#version 330 core
layout (location = 0) in vec3 vertex; 
layout (location = 1) in vec2 uvs; 

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
	mat4 view;
} matrices;

vec4 color;

void main()
{
    gl_Position = matrices.projection * matrices.view * matrices.model * vec4(vertex, 1.0f);
}  