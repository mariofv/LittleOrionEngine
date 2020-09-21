layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 PostProcessFilter;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
	FragColor =  texture(skybox, TexCoords);
	PostProcessFilter = vec4(0.0, 1.0, 0.0, 0.0);
}
