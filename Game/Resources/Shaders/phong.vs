#version 330

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv0;
layout(location = 2) in vec3 vertex_normal;

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
	mat4 view;
};

struct Material {
	sampler2D diffuse_map;
	vec4 diffuse_color;
	float k_diffuse;
	sampler2D specular_map;
	vec4 specular_color;
	float shininess;
	float k_specular;
	sampler2D occlusion_map;
	float k_ambient;
	sampler2D emissive_map;
	vec4 emissive_color;
};
uniform Material material;

out vec2 texCoord;
out vec3 position;
out vec3 normal;

void main()
{
	gl_Position = proj*view*model*vec4(vertex_position, 1.0);
	texCoord = vertex_uv0;
	position = (model*vec4(vertex_position, 1.0)).xyz;
	normal = (model*vec4(vertex_normal, 0.0)).xyz;
}
