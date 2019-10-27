#version 330
layout(location = 0) in vec3 vertex_position;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform float u_time;

void main()
{
  gl_Position = proj*view*model*vec4(vertex_position, 1.0);
  gl_Position.x = gl_Position.x + sin(u_time/128 + gl_Position.y*100);
}
