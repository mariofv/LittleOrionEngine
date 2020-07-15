layout(location = 0)in vec3 vertex; 
layout(location = 1)in vec2 uvs;

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
	mat4 view;
} matrices;

vec4 color;
out vec3 col;
out vec2 texCoord;


void main()
{
    gl_Position = matrices.proj * matrices.view * vec4(vertex, 1.0f);
	col = vec3(0.0f, 0.0f, 1.0f);
	texCoord =  uvs;
}  