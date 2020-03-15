#version 330

layout(location = 0) in vec3 vertex_position;

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
	mat4 view;
} matrices;

out vec2 frag_position;;

void main()
{
  mat4 translate_matrix = mat4(
   1.0, 0.0, 0.0, 0.0, // first column (not row!)
   0.0, 1.0, 0.0, 0.0, // second column
   0.0, 0.0, 1.0, 0.0, // third column
   0.0, 0.0, 0.0, 1.0  // fourth column
);

  vec4 fragment_position = matrices.proj * matrices.view * translate_matrix * vec4(vertex_position, 1.0);
  frag_position = vertex_position.xz;
  gl_Position = fragment_position;
}
