layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv0;
layout(location = 7) in vec2 vertex_uv1;
layout(location = 2) in vec3 vertex_normal;
layout(location = 3) in vec3 vertex_tangent;
layout(location = 4) in uvec4 vertex_joints;
layout(location = 5) in vec3 vertex_weights;
layout(location = 6) in uint vertex_num_joints;


layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
	mat4 view;
} matrices;

uniform int num_joints;
uniform int time;

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

//SHADOWS
uniform mat4 close_directional_view;
uniform mat4 close_directional_proj;

uniform mat4 mid_directional_view;
uniform mat4 mid_directional_proj;

uniform mat4 far_directional_view;
uniform mat4 far_directional_proj;

out vec4 close_pos_from_light;
out vec4 mid_pos_from_light;
out vec4 far_pos_from_light;
out vec3 vertex_normal_fs;
out vec3 vertex_tangent_fs;
uniform float render_depth_from_light;

out float distance_to_camera;

void main()
{
	mat4 close_lightSpaceMatrix = close_directional_proj * close_directional_view;
	mat4 mid_lightSpaceMatrix   = mid_directional_proj * mid_directional_view;
	mat4 far_lightSpaceMatrix   = far_directional_proj * far_directional_view;

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


	//Light space
	close_pos_from_light = close_lightSpaceMatrix*vec4(position, 1.0);
	mid_pos_from_light = mid_lightSpaceMatrix*vec4(position, 1.0);
	far_pos_from_light = far_lightSpaceMatrix*vec4(position, 1.0);

	vec4 eye_coordinate_pos = matrices.view * matrices.model * skinning_matrix * vec4(vertex_position, 1.0);
	distance_to_camera = -eye_coordinate_pos.z;

	gl_Position = matrices.proj * eye_coordinate_pos;
}
