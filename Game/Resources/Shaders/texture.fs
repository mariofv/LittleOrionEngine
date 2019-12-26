#version 330 core

in vec2 texCoord;

out vec4 FragColor;

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

	bool use_diffuse_map;
};

uniform Material material;

void main()
{
    vec4 diffuse_color  = texture(material.diffuse_map, texCoord);    
	vec4 specular_color  = texture(material.specular_map, texCoord);    
	vec3 occlusion_color = texture(material.occlusion_map, texCoord).rgb;    
	vec3 emissive_color  = texture(material.emissive_map, texCoord).rgb;
         
    vec3 color = emissive_color+  // emissive                
	diffuse_color.rgb*(occlusion_color)+ // ambient                 
	diffuse_color.rgb+ // diffuse
	specular_color.rgb; // specular

    FragColor = vec4(color, 1.0);
}


vec4 get_diffuse_color(const Material mat, const vec2 texCoord)
{     
	if(mat.use_diffuse_map)
	{        
		return texture(mat.diffuse_map, texCoord)*mat.diffuse_color;    
	}    
	return mat.diffuse_color;
}