#version 330

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv0;
layout(location = 2) in vec3 vertex_normal;

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
  mat4 view;
} matrices;

out vec2 texCoord;
out vec3 normal;
out vec3 FragPos;
out vec3 viewPos;

void main()
{
  gl_Position = matrices.proj*matrices.view*matrices.model*vec4(vertex_position, 1.0);
  FragPos = vec3(matrices.model * vec4(vertex_position, 1.0));
  texCoord = vertex_uv0;
  normal =(matrices.model*vec4(vertex_normal, 0.0)).xyz;
  viewPos = transpose(mat3(matrices.view))*(-matrices.view[3].xyz);
}
