#version 430

//Commit addition (sorry)

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

out float distance_to_camera;

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

	position = (matrices.model*skinning_matrix*vec4(vertex_position, 1.0)).xyz;
	normal = (matrices.model*skinning_matrix*vec4(vertex_normal, 0.0)).xyz;
	tangent = (matrices.model*skinning_matrix*vec4(vertex_tangent, 0.0)).xyz;

	view_pos    = transpose(mat3(matrices.view)) * (-matrices.view[3].xyz);
	view_dir    = normalize(view_pos - position);



	//Tangent space matrix
	mat4 modelview = matrices.model * matrices.view;
	mat3 normalMatrix = mat3(matrices.model);
    vec3 T = normalize(normalMatrix * vertex_tangent);
    vec3 N = normalize(normalMatrix * vertex_normal);
    T = normalize(T - N *dot(T, N));
    vec3 B = cross(N, T);

	TBN = transpose(mat3(T, B, N));


	vec4 eye_coordinate_pos = matrices.view * matrices.model * skinning_matrix * vec4(vertex_position, 1.0);
	distance_to_camera = -eye_coordinate_pos.z;

	gl_Position = matrices.proj * eye_coordinate_pos;

}

mat3 CreateTangentSpace(const vec3 normal, const vec3 tangent)
{
	vec3 ortho_tangent = normalize(tangent-dot(tangent, normal)*normal); // Gram-Schmidt
	vec3 bitangent = cross(normal, ortho_tangent);
	return mat3(tangent, bitangent, normal);
}
