layout(location = 0) in vec3 vertex_position;

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
	mat4 view;
} matrices;

out vec2 frag_position;

void main()
{
  vec4 fragment_position = matrices.proj * matrices.view * matrices.model * vec4(vertex_position, 1.0);
  frag_position =  (matrices.model *  vec4(vertex_position, 1.0)).xz;
  gl_Position = fragment_position;
}
