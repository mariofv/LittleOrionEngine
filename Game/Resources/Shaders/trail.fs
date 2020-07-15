in vec3 col;
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D tex;
uniform vec4 color;
uniform float bloom_intensity;

void main()
{
	vec4 texture_color = texture(tex, texCoord) * color * bloom_intensity;
	FragColor =  texture_color;

}  