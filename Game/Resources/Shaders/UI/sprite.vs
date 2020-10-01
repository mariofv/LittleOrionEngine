layout (location = 0) in vec3 vertex_position; // <vec2 position, vec2 texCoords>
layout (location = 1) in vec2 vertex_uv; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    TexCoords = vertex_uv;
    gl_Position = projection * model * vec4(vertex_position, 1.0);
}
