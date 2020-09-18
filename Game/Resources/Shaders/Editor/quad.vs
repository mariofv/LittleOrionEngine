layout(location = 0) in vec3 vertex_pos;
layout(location = 1) in vec2 vertex_uv;

out vec2 texCoord;

void main()
{
  gl_Position = vec4(vertex_pos, 1.0);
	texCoord =  vertex_uv;
}
