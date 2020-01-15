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
};
uniform Billboard billboard;

out vec2 texCoord;

void main()
{
  vec3 camera_right = vec3(matrices.view[0][0], matrices.view[1][0], matrices.view[2][0]);
  vec3 camera_up = vec3(matrices.view[0][1], matrices.view[1][1], matrices.view[2][1]);
  vec3 billboard_vertex_world_position =
    billboard.center_pos
    + camera_right * vertex_position.x * billboard.size
    + camera_up * vertex_position.y * billboard.size;

	gl_Position = vec4(billboard_vertex_world_position, 1.0);
	texCoord = vertex_uv0;

}
