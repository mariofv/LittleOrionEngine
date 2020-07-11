struct Trail
{
  vec4 position;
  vec4 uv;
};

in vec4 vertex; 

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
    gl_Position = matrices.proj * matrices.view * vertex;
	col = vec3(0.0f, 0.0f, 1.0f);
	texCoord =  vec2(0.0f, 0.0f);
}  