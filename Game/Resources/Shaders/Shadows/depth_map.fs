in vec2 texCoord;

layout (location = 0) out vec4 FragColor;

uniform sampler2D tex;

void main()
{
	float depth_value = texture(tex, texCoord).r;
	FragColor = vec4(vec3(depth_value), 1.0);
}
