#version 330 core

in vec2 texCoord;

out vec4 FragColor;

struct Billboard {
  vec3 center_pos;
  float width;
  float height;
  sampler2D texture;
};
uniform Billboard billboard;

void main()
{
	vec4 texture_color = texture(billboard.texture, texCoord);
	FragColor =  texture_color;
}
