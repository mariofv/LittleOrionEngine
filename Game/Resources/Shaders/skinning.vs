#version 430

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv0;
layout(location = 2) in vec3 vertex_normal;
layout(location = 3) in vec4 vertex_joints;
layout(location = 4) in vec4 vertex_weights;
layout(location = 5) in uint vertex_num_joints;

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
  mat4 view;
  mat4 pallete[64];
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
};
uniform Material material;

out vec2 texCoord;
out vec3 position;
out vec3 normal;

void main()
{

	mat4 skinning_matrix = mat4(0);
    for(int i=0; i<vertex_num_joints; i++)
	{
        skinning_matrix += in_weight[i] * pallete[in_joint[i]];
	}
	gl_Position = matrices.proj*matrices.view* matrices.model;
	texCoord = vertex_uv0;
	position = (matrices.model*vec4(vertex_position, 1.0)).xyz;
	normal = (matrices.model*vec4(vertex_normal, 0.0)).xyz;
}
