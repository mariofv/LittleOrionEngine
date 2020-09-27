in vec2 texCoord;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec4 PostProcessFilter;

uniform sampler2D tex;
uniform vec4 color;
uniform vec4 color_blend;
uniform float emissive_intensity;
uniform float percentage;

void main()
{
	vec2 t = texCoord;
	float f = fract(t.s);
	vec4 texture_color = texture(tex, texCoord) * color * emissive_intensity;
	vec4 texture_color2 = texture(tex, texCoord) * color_blend * emissive_intensity;
	if(f < percentage)
	FragColor = texture_color;
	else
	FragColor = texture_color2;
	
	
	float brightness = dot(texture_color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0)
	{
		BrightColor = vec4(texture_color.rgb, 1.0);
	}
	else
	{
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	PostProcessFilter = vec4(1.0, 0.0, 0.0, 0.0);
}
