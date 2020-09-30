in vec2 texCoord;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec4 PostProcessFilter;

uniform sampler2D tex;
uniform vec4 color;
uniform vec4 color_blend;
uniform float emissive_intensity;
uniform float percentage;
uniform float smooth_step;

void main()
{
	vec2 t = texCoord;
	float f = fract(t.s);
	float s_step = smoothstep(percentage, percentage + smooth_step, f);
	
	vec4 texture_color = texture(tex, texCoord) * vec4(mix(color_blend, color, s_step)) * emissive_intensity;

	FragColor = texture_color;
	
	if(texture_color.a <0.1)
	{
			discard;
	}
	
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
