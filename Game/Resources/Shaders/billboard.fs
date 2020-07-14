struct Billboard
{
  sampler2D texture;
  sampler2D texture_emissive;
  vec4 color;
  vec4 color_emissive;

  int num_columns;
  int num_rows;
  int current_sprite_x;
  int current_sprite_y;
  int emissive_intensity;
};
uniform Billboard billboard;

in vec2 texCoord;
out vec4 FragColor;

void main()
{
	// Change to preprocessor directives
		vec4 texture_color = texture(billboard.texture, texCoord)*billboard.color;
		texture_color += texture(billboard.texture_emissive, texCoord)*billboard.color_emissive * billboard.emissive_intensity;
		if(texture_color.a <0.1)
		{
			discard;
		}
		FragColor =  texture_color;
}
