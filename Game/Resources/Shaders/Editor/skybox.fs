layout (location = 0) out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
	FragColor =  texture(skybox, TexCoords);
	FragColor.rgb = pow(FragColor.rgb, vec3(2.2));
}
