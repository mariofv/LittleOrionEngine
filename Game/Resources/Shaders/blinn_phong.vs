#version 430

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

struct Material {
	sampler2D diffuse_map;
	vec4 diffuse_color;
	float k_diffuse;
	sampler2D specular_map;
	vec4 specular_color;
	float k_specular;
	sampler2D occlusion_map;
	float k_ambient;
	sampler2D emissive_map;
	vec4 emissive_color;
	sampler2D normal_map;

	float roughness;
	float metalness;
	float transparency;
	float tiling_x;
	float tiling_y;
	bool use_normal_map;


};
uniform Material material;

out vec2 texCoord;
out vec3 position;
out vec3 normal;
out vec3 tangent;


void main()
{
	texCoord = vertex_uv0;
	position = (matrices.model*vec4(vertex_position, 1.0)).xyz;
	normal = (matrices.model*vec4(vertex_normal, 0.0)).xyz;
	tangent = (matrices.model*vec4(vertex_tangent, 0.0)).xyz;

	gl_Position = matrices.proj*matrices.view*vec4(position, 1.0);
}
