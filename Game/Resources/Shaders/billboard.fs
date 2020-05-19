#version 430 core


in vec2 texCoord;
in int isSpritesheet;

in vec2 frame;

out vec4 FragColor;

struct Billboard {
  vec3 center_pos;
  float width;
  float height;
  sampler2D texture;
  bool isSpritesheet;

  vec4 color;

  int XTiles;
  int YTiles;
  float speed;
  int alignment_type;

};




uniform Billboard billboard;


void main()
{
	// Change to preprocessor directives
	if(!billboard.isSpritesheet)
	{
		vec4 texture_color = texture(billboard.texture, texCoord)*billboard.color;
		if(texture_color.a <0.1)
		{
			discard;
		}
		FragColor =  texture_color;
	}

	else
	{
		vec4 texture_color = texture(billboard.texture, frame)*billboard.color;
		if(texture_color.a <0.1)
		{
			discard;
		}
		FragColor =  texture_color;
	}

}
