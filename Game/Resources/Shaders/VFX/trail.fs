in vec2 texCoord;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform sampler2D tex;
uniform vec4 color;
uniform float emissive_intensity;

void main()
{
	vec4 texture_color = texture(tex, texCoord) * color * emissive_intensity;
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
