struct Billboard
{
  sampler2D texture;
  vec4 color;

  int num_columns;
  int num_rows;
  int current_sprite_x;
  int current_sprite_y;
};
uniform Billboard billboard;

in vec2 texCoord;
out vec4 FragColor;

void main()
{
	// Change to preprocessor directives
		vec4 texture_color = texture(billboard.texture, texCoord)*billboard.color;
		if(texture_color.a <0.1)
		{
			discard;
		}
		FragColor =  texture_color;
}
