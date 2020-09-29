layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

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
