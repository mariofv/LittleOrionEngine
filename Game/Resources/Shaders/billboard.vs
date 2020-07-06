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
  sampler2D texture;
  vec4 color;

  bool is_spritesheet;
  int num_columns;
  int num_rows;
  int current_sprite_x;
  int current_sprite_y;
};
uniform Billboard billboard;

out vec2 texCoord;

void main()
{
  gl_Position = matrices.proj * matrices.view * matrices.model;

  vec2 vertex_uvs = vertex_uv0;

#if ENABLE_SPRITESHEET
  float u = mix(current_sprite_x, current_sprite_x + 1, vertex_uv0.x) / num_rows;
  float v = mix(current_sprite_x, current_sprite_x + 1, vertex_uv0.x) / num_columns;
  vertex_uvs = vec2(u, v);
#endif

  texCoord = vertex_uvs;
}
