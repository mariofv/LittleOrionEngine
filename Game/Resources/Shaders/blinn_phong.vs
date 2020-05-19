#version 430

//Commit addition (sorry)

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv0;
layout(location = 2) in vec3 vertex_normal;
layout(location = 3) in vec3 vertex_tangent;

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
	mat4 view;
} matrices;

out vec2 texCoord;
out vec3 position;
out vec3 normal;
out vec3 tangent;
out mat3 TBN;

//Normal mapping
mat3 CreateTangentSpace(const vec3 normal, const vec3 tangent);

//SHADOWS
uniform mat4 close_directional_view;
uniform mat4 close_directional_proj;

uniform mat4 mid_directional_view;
uniform mat4 mid_directional_proj;

uniform mat4 far_directional_view;
uniform mat4 far_directional_proj;

uniform mat4 main_cam_proj;
uniform mat4 main_cam_view;
out vec4 pos_from_main_camera;

out vec4 close_pos_from_light;
out vec4 mid_pos_from_light;
out vec4 far_pos_from_light;
uniform float render_depth_from_light;


void main()
{

	mat4 close_lightSpaceMatrix = close_directional_proj * close_directional_view;
	mat4 mid_lightSpaceMatrix   = mid_directional_proj * mid_directional_view;
	mat4 far_lightSpaceMatrix   = far_directional_proj * far_directional_view;
	mat4 main_cam_space			= main_cam_proj * main_cam_view;


// General variables
	texCoord = vertex_uv0;
	position = (matrices.model*vec4(vertex_position, 1.0)).xyz;
	normal = (matrices.model*vec4(vertex_normal, 0.0)).xyz;
	tangent = (matrices.model*vec4(vertex_tangent, 0.0)).xyz;


//Tangent space matrix
	mat4 modelview = matrices.model * matrices.view;
	mat3 normalMatrix = mat3(matrices.model);
    vec3 T = normalize(normalMatrix * vertex_tangent);
    vec3 N = normalize(normalMatrix * vertex_normal);
    T = normalize(T - N *dot(T, N));
    vec3 B = cross(N, T);

	TBN = transpose(mat3(T, B, N));  

//Light space
	close_pos_from_light = close_lightSpaceMatrix*matrices.model*vec4(vertex_position, 1.0);
	mid_pos_from_light = mid_lightSpaceMatrix*matrices.model*vec4(vertex_position, 1.0);
	far_pos_from_light = far_lightSpaceMatrix*matrices.model*vec4(vertex_position, 1.0);
	pos_from_main_camera = main_cam_space * matrices.model*vec4(vertex_position, 1.0);
	gl_Position = matrices.proj*matrices.view*vec4(position, 1.0);

}

mat3 CreateTangentSpace(const vec3 normal, const vec3 tangent)
{
	vec3 ortho_tangent = normalize(tangent-dot(tangent, normal)*normal); // Gram-Schmidt
	vec3 bitangent = cross(normal, ortho_tangent);
	return mat3(tangent, bitangent, normal);
}