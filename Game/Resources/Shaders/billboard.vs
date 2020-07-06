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

  int num_columns;
  int num_rows;
  int current_sprite_x;
  int current_sprite_y;
};
uniform Billboard billboard;

out vec2 texCoord;

mat4x4 GetModelViewMatrix();

void main()
{
  mat4x4 model_view_matrix = GetModelViewMatrix();

  gl_Position = matrices.proj *  model_view_matrix * vec4(vertex_position, 1.0);

  vec2 vertex_uvs = vertex_uv0;

#if ENABLE_SPRITESHEET
  float u = mix(current_sprite_x, current_sprite_x + 1, vertex_uv0.x) / num_rows;
  float v = mix(current_sprite_x, current_sprite_x + 1, vertex_uv0.x) / num_columns;
  vertex_uvs = vec2(u, v);
#endif

  texCoord = vertex_uvs;
}

mat4x4 GetModelViewMatrix()
{
  mat4x4 model_view = matrices.view * matrices.model;
  #if ENABLE_BILLBOARD_VIEWPOINT_ALIGNMENT
      model_view[0][0] = matrices.model[0][0];
      model_view[0][1] = 0.0;
      model_view[0][2] = 0.0;

      model_view[1][0] = 0.0;
      model_view[1][1] = matrices.model[1][1];
      model_view[1][2] = 0.0;

      model_view[2][0] = 0.0;
      model_view[2][1] = 0.0;
      model_view[2][2] = matrices.model[2][2];

  #elif ENABLE_BILLBOARD_AXIAL_ALIGNMENT
      model_view[0][0] = matrices.model[0][0];
      model_view[0][1] = 0.0;
      model_view[0][2] = 0.0;

      model_view[2][0] = 0.0;
      model_view[2][1] = 0.0;
      model_view[2][2] = matrices.model[2][2];

  #endif
  return model_view;
}
