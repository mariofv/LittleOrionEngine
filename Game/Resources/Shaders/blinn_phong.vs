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
uniform mat4 directional_view;
uniform mat4 directional_proj;
out vec4 pos_from_light;


void main()
{

	mat4 lightSpaceMatrix = directional_proj * directional_view;

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

	pos_from_light = lightSpaceMatrix*matrices.model*vec4(position, 1.0);
	gl_Position = matrices.proj*matrices.view*vec4(position, 1.0);
}

mat3 CreateTangentSpace(const vec3 normal, const vec3 tangent)
{
	vec3 ortho_tangent = normalize(tangent-dot(tangent, normal)*normal); // Gram-Schmidt
	vec3 bitangent = cross(normal, ortho_tangent);
	return mat3(tangent, bitangent, normal);
}