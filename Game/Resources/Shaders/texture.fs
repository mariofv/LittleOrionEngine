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
	vec3 occlusion_color = get_occlusion_color(material, texCoord);    
	vec3 emissive_color  = get_emissive_color(material, texCoord);
  	
    // diffuse 
	vec3 lightDir = light.light_position - FragPos;
	float diffuse = lambert(lightDir, normal);
     
	//specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normalize(normal));  
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 result = emissive_color
	+diffuse_color.rgb*(occlusion_color*material.k_ambient)
	+diffuse_color.rgb*diffuse*material.k_diffuse
	+specular_color.rgb*specular*material.k_specular;
    FragColor = vec4(result,1.0);
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
	if(mat.use_occlusion_map)
	{ 
		return texture(mat.occlusion_map, texCoord).rgb;  
	}
	return vec3(0.0,0.0,0.0);
		
}
vec3 get_emissive_color(const Material mat, const vec2 texCoord)
{     
	if(mat.use_emissive_map)
	{     
		return (texture(mat.emissive_map, texCoord)*mat.emissive_color).rgb;    
	}    
	return mat.emissive_color.rgb;
}

float lambert(vec3 direcction,vec3 norm)
{
  vec3 normal   = normalize(norm);
  vec3 lightDir = normalize(direcction);
  float diffuse = max(0.0, dot(normal, lightDir));
  return diffuse;
}
