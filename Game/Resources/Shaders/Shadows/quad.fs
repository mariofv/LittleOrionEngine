in vec2 texCoord;

layout (location = 0) out vec4 FragColor;

uniform sampler2D tex;

void main()
{
	vec3 tex_color = texture(tex, texCoord).rgb;
	float znear = 10;
	float zfar = 100;
	float linear_depth = (2.0 * znear) / (zfar + znear - tex_color.x * (zfar - znear));
	FragColor = vec4(linear_depth, linear_depth, linear_depth, 1.0);
}
