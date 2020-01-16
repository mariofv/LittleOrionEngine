#version 330

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv0;

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
	mat4 view;
} matrices;

struct Billboard
{
  vec3 center_pos;
  float size;
  sampler2D texture;
};
uniform Billboard billboard;

out vec2 texCoord;

void main()
{
  gl_Position = matrices.proj*(matrices.view*vec4(billboard.center_pos,1.0) + vec4(billboard.size*vertex_position, 0.0));
  texCoord = vertex_uv0;
}
