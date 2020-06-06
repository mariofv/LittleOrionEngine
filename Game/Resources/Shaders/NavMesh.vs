#version 430 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec4 vertex_color;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
out vec4 fcolor;

void main() {
    fcolor = vertex_color;
    gl_Position = proj*view*model*vec4(vertex_position, 1.0);
}