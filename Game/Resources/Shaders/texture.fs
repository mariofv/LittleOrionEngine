#version 330 core

in vec2 texCoord;

out vec4 color;

struct Material {    
	sampler2D diffuse_map;    
	vec4 diffuse_color;   
	float k_diffuse;    
	sampler2D specular_map;    
	vec3 specular_color;    
	float shininess;   
	float k_specular;   
	sampler2D occlusion_map;    
	float k_ambient;    
	sampler2D emissive_map;    
	vec3 emissive_color;
};

uniform Material material;

void main()
{
    color = texture(material.diffuse_map, texCoord);
}
