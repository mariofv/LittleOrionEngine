layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv0;
layout(location = 2) in vec3 vertex_normal;

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
  mat4 view;
} matrices;

void main()
{
  gl_Position = matrices.proj*matrices.view*matrices.model*vec4(vertex_position, 1.0);
}
