#version 430

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv0;
layout(location = 2) in vec3 vertex_normal;
layout(location = 3) in vec3 vertex_tangent;
layout(location = 4) in uvec4 vertex_joints;
layout(location = 5) in vec4 vertex_weights;

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
uniform mat4 palette[64];

out vec2 texCoord;
out vec3 position;
out vec3 normal;
out vec3 tangent;
out mat3 TBN;

//Without tangent modification
out vec3 view_pos;
out vec3 view_dir;

//With tangent modification
out vec3 t_view_pos;
out vec3 t_frag_pos;

//Normal mapping
mat3 CreateTangentSpace(const vec3 normal, const vec3 tangent);

void main()
{

	mat4 skinning_matrix = mat4(0);
    for(uint i=0; i<4; i++)
	{
		skinning_matrix += vertex_weights[i] * palette[vertex_joints[i]];
	}
	gl_Position = matrices.proj * matrices.view * matrices.model * skinning_matrix * vec4(vertex_position, 1.0);
	texCoord = vertex_uv0;
	position = (matrices.model * skinning_matrix * vec4(vertex_position, 1.0)).xyz;
	normal = (matrices.model * skinning_matrix * vec4(vertex_normal, 0.0)).xyz;	
	tangent = (matrices.model*vec4(vertex_tangent, 0.0)).xyz;

	view_pos    = transpose(mat3(matrices.view)) * (-matrices.view[3].xyz);
	view_dir    = normalize(view_pos - position);
	
	//Tangent space matrix
	mat3 normalMatrix = mat3(matrices.model);
	vec3 T = normalize(normalMatrix * vertex_tangent);
	vec3 N = normalize(normalMatrix * vertex_normal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	TBN = transpose(mat3(T, B, N));  

	//Computing tangent variables for normal mapping
	t_view_pos = TBN * view_pos;
	t_frag_pos = TBN * position;
}

mat3 CreateTangentSpace(const vec3 normal, const vec3 tangent)
{
	vec3 ortho_tangent = normalize(tangent-dot(tangent, normal)*normal); // Gram-Schmidt
	vec3 bitangent = cross(normal, ortho_tangent);
	return mat3(tangent, bitangent, normal);
}