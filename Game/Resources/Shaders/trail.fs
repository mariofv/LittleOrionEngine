in vec3 col;
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D tex;
uniform vec2 _color;

void main()
{
	vec4 texture_color = texture(tex, texCoord);
	FragColor =  texture_color;

}  