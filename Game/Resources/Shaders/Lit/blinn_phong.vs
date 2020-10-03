layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv0;
layout(location = 7) in vec2 vertex_uv1;
layout(location = 2) in vec3 vertex_normal;
layout(location = 3) in vec3 vertex_tangent;
layout(location = 4) in uvec4 vertex_joints;
layout(location = 5) in vec4 vertex_weights;
layout(location = 6) in uint vertex_num_joints;


layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
	mat4 view;
} matrices;

layout (std140) uniform DepthMatrices
{
  mat4 full_depth_space_matrix;
  mat4 near_depth_space_matrix;
  mat4 mid_depth_space_matrix;
  mat4 far_depth_space_matrix;
} depth_matrices;

uniform int num_joints;

uniform mat4 palette[128]; // REMEMBER MAXIMUM NUMBER OF BONES NOT MORE PLEASE DON'T LOSE YOUR TIME LIKE ME
uniform int has_skinning_value;

out vec2 texCoord;
out vec2 texCoordLightmap;
out vec3 position;
out vec3 normal;
out mat3 TBN;

//Without tangent modification
out vec3 view_pos;
out vec3 view_dir;

out vec4 position_near_depth_space;
out vec4 position_mid_depth_space;
out vec4 position_far_depth_space;
out vec4 position_full_depth_space;

out vec3 vertex_normal_fs;
out vec3 vertex_tangent_fs;

void main()
{
//Skinning
	mat4 skinning_matrix = mat4(has_skinning_value);
  for(uint i=0; i<vertex_num_joints; i++)
  {
    skinning_matrix += vertex_weights[i] * palette[vertex_joints[i]];
  }

// General variables
	texCoord = vertex_uv0;
	texCoordLightmap = vertex_uv1;
	vertex_normal_fs =vertex_normal;
	vertex_tangent_fs =vertex_tangent;
	position = (matrices.model*skinning_matrix*vec4(vertex_position, 1.0)).xyz;
	normal = (matrices.model*skinning_matrix*vec4(vertex_normal, 0.0)).xyz;

	view_pos    = transpose(mat3(matrices.view)) * (-matrices.view[3].xyz);
	view_dir    = normalize(view_pos - position);

	vec4 eye_coordinate_pos = matrices.view * matrices.model * skinning_matrix * vec4(vertex_position, 1.0);

	gl_Position = matrices.proj * eye_coordinate_pos;

  //Light space
  position_near_depth_space = depth_matrices.near_depth_space_matrix * matrices.model * skinning_matrix * vec4(vertex_position, 1.0);
  position_mid_depth_space = depth_matrices.mid_depth_space_matrix * matrices.model * skinning_matrix * vec4(vertex_position, 1.0);
  position_far_depth_space = depth_matrices.far_depth_space_matrix * matrices.model * skinning_matrix * vec4(vertex_position, 1.0);
  position_full_depth_space = depth_matrices.full_depth_space_matrix * matrices.model * skinning_matrix * vec4(vertex_position, 1.0);
}
