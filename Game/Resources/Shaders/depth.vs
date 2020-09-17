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

uniform int num_joints;

uniform mat4 palette[128]; // REMEMBER MAXIMUM NUMBER OF BONES NOT MORE PLEASE DON'T LOSE YOUR TIME LIKE ME
uniform int has_skinning_value;

void main()
{
//Skinning
	mat4 skinning_matrix = mat4(has_skinning_value);
  for(uint i=0; i<vertex_num_joints; i++)
  {
    skinning_matrix += vertex_weights[i] * palette[vertex_joints[i]];
  }

	gl_Position = matrices.proj * matrices.view * matrices.model * skinning_matrix * vec4(vertex_position, 1.0);
}
