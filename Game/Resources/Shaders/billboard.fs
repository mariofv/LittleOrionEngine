#version 330 core

in vec2 texCoord;

out vec4 FragColor;

struct Billboard {
	sampler2D texture;
};
uniform Billboard billboard;

void main()
{
	FragColor = texture(billboard.texture, texCoord);
}
