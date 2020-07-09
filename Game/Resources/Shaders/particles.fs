out vec4 FragColor;

in vec2 texCoord;
in vec4 color;

struct Billboard
{
  sampler2D texture;

  int num_columns;
  int num_rows;
};
uniform Billboard billboard;

void main()
{
	vec4 texture_color = texture(billboard.texture, texCoord)*color;
	if(texture_color.a <0.1)
	{
		discard;
	}
	FragColor =  texture_color;
}
