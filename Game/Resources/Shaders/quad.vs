layout(location = 0)in vec4 vertex;

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
	mat4 view;
} matrices;

out vec2 texCoord;

void main()
{
  gl_Position = vec4(vertex.x, vertex.y, 0.0, 1.0);
	texCoord =  vertex.zw;
}
