in vec3 col;
in vec2 texCoord;
out vec4 FragColor;

uniform vec2 _color;

void main()
{

	FragColor =  vec4(col, 1.0);

}  