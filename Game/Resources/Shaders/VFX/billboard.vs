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
  sampler2D texture_emissive;
  vec4 color;
   vec4 color_emissive;

  int num_columns;
  int num_rows;
  int current_sprite_x;
  int current_sprite_y;
  int emissive_intensity;
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
  float u = mix(billboard.current_sprite_x, billboard.current_sprite_x + 1, vertex_uv0.x) / billboard.num_columns;
  float v = mix(billboard.current_sprite_y, billboard.current_sprite_y + 1, vertex_uv0.y) / billboard.num_rows;
  vertex_uvs = vec2(u, v);
#endif

  texCoord = vertex_uvs;
}

mat4x4 GetModelViewMatrix()
{
  mat4x4 model_view = matrices.view * matrices.model;
  #if ENABLE_BILLBOARD_VIEWPOINT_ALIGNMENT
      float x_scale = length(model_view[0].xyz);
      model_view[0][0] = x_scale;
      model_view[0][1] = 0.0;
      model_view[0][2] = 0.0;

      float y_scale = length(model_view[1].xyz);
      model_view[1][0] = 0.0;
      model_view[1][1] = y_scale;
      model_view[1][2] = 0.0;

      float z_scale = length(model_view[2].xyz);
      model_view[2][0] = 0.0;
      model_view[2][1] = 0.0;
      model_view[2][2] = z_scale;

  #elif ENABLE_BILLBOARD_AXIAL_ALIGNMENT
      float x_scale = length(model_view[0].xyz);
      model_view[0][0] = x_scale;
      model_view[0][1] = 0.0;
      model_view[0][2] = 0.0;

      float z_scale = length(model_view[2].xyz);
      model_view[2][0] = 0.0;
      model_view[2][1] = 0.0;
      model_view[2][2] = z_scale;

  #endif
  return model_view;
}
