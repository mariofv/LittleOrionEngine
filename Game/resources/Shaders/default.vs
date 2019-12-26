#version 330

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 uv0;
layout(location = 2) in vec3  norm;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec2 texCoord;
out vec3 normal;

void main()
{
  gl_Position = proj*view*model*vec4(vertex_position, 1.0);
  texCoord = uv0;
  normal = norm;
}
