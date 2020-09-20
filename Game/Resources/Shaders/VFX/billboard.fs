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
  float emissive_intensity;
};
uniform Billboard billboard;

in vec2 texCoord;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

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

    float brightness = dot(texture_color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
    {
        BrightColor = vec4(texture_color.rgb, 1.0);
    }
    else
    {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
