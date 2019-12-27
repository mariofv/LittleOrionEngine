#version 330 core

in vec2 texCoord;
in vec3 normal;
in vec3 FragPos;

out vec4 FragColor;

struct Material {    
	sampler2D diffuse_map;    
	vec4 diffuse_color;   
	float k_diffuse;    
	sampler2D specular_map;    
	vec4 specular_color;    
	float shininess;   
	float k_specular;   
	sampler2D occlusion_map;    
	float k_ambient;    
	sampler2D emissive_map;    
	vec4 emissive_color;

	bool use_diffuse_map;
	bool use_emissive_map;
	bool use_specular_map;
	bool use_occlusion_map;
};
uniform Material material;

struct Light{
	vec3 light_color;
	vec3 light_position;
	float light_intensity;
};
uniform Light light;

uniform vec3 viewPos;

vec4 get_diffuse_color(const Material mat, const vec2 texCoord);
vec3 get_occlusion_color(const Material mat, const vec2 texCoord);
vec3 get_emissive_color(const Material mat, const vec2 texCoord);
vec4 get_specular_color(const Material mat, const vec2 texCoord);
float lambert(vec3 light_position,vec3 norm);

void main()
{
    vec4 diffuse_color  = get_diffuse_color(material, texCoord);    
	vec4 specular_color  = get_specular_color(material, texCoord);    
	//vec3 occlusion_color = get_occlusion_color(material, texCoord).rgb;    
	//vec3 emissive_color  = get_emissive_color(material, texCoord).rgb;
        
  	
    // ambient
    vec3 ambient = light.light_intensity * light.light_color;
  	
    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.light_position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}


vec4 get_diffuse_color(const Material mat, const vec2 texCoord)
{     
	if(mat.use_diffuse_map)
	{     
		return texture(mat.diffuse_map, texCoord)*mat.diffuse_color;    
	}    
	return mat.diffuse_color;
}

vec4 get_specular_color(const Material mat, const vec2 texCoord)
{     
	if(mat.use_specular_map)
	{     
		return texture(mat.specular_map, texCoord)*mat.specular_color;    
	}    
	return mat.specular_color;
}
vec3 get_occlusion_color(const Material mat, const vec2 texCoord)
{     

	return texture(mat.occlusion_map, texCoord).rgb;  
		
}
vec3 get_emissive_color(const Material mat, const vec2 texCoord)
{     
	if(mat.use_emissive_map)
	{     
		return (texture(mat.emissive_map, texCoord)*mat.emissive_color).rgb;    
	}    
	return mat.emissive_color.rgb;
}

float lambert(vec3 light_position,vec3 norm)
{
  vec3 normal    = normalize(norm);
  vec3 light_dir = normalize(light_position);
  float diffuse  = max(0.0, dot(normal, light_dir));
  return diffuse;
}
