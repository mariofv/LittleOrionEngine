#version 430 core

out vec4 FragColor;
  
in vec2 texCoord;
in vec2 frame;
in vec4 color;

struct Billboard
{
  bool isSpritesheet;
  int XTiles;
  int YTiles;

  sampler2D texture;
};
uniform Billboard billboard;

void main()
{
	vec2 coordinates = !billboard.isSpritesheet ? texCoord : frame;
	vec4 texture_color = texture(billboard.texture, coordinates)*color;
	if(texture_color.a <0.1)
	{
		discard;
	}
	FragColor =  texture_color;

}