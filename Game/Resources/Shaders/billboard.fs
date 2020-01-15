#version 330 core

in vec2 texCoord;

out vec4 FragColor;

struct Billboard {
  vec3 center_pos;
  float size;
  sampler2D texture;
};
uniform Billboard billboard;

void main()
{
	vec4 texture_color = texture(billboard.texture, texCoord);
	texture_color = mix(texture_color, vec4(1.0), texture_color.w);
	FragColor =  texture_color;
}
