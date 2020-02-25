#version 330 core

in vec3 position;
in vec3 normal;
in vec2 texCoord;

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
};
uniform Material material;

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
  mat4 view;
} matrices;

layout (std140) uniform DirectionalLight
{
	vec3 color;
	vec3 direction;
	int num_directional_lights;
} directional_light;

struct PointLight
{
	vec3 color;
	vec3 position;

	float constant;
    float linear;
    float quadratic;
};
struct SpotLight 
{
	vec3 color;
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;     
};


uniform int num_spot_lights;
uniform SpotLight spot_lights[10];

uniform int num_point_lights;
uniform PointLight point_lights[10];


vec4 GetDiffuseColor(const Material mat, const vec2 texCoord);
vec4 GetSpecularColor(const Material mat, const vec2 texCoord);
vec3 GetOcclusionColor(const Material mat, const vec2 texCoord);
vec3 GetEmissiveColor(const Material mat, const vec2 texCoord);

vec3 CalculateDirectionalLight(const vec3 normalized_normal);
vec3 CalculateSpotLight(SpotLight spot_light, const vec3 normalized_normal);
vec3 CalculatePointLight(PointLight point_light, const vec3 normalized_normal);

void main()
{
	vec3 normalized_normal = normalize(normal);

	vec3 result = vec3(0);
	for (int i = 0; i < directional_light.num_directional_lights; ++i)
	{
		result += CalculateDirectionalLight(normalized_normal);
	}
	for (int i = 0; i < num_point_lights; ++i)
	{
		result += CalculatePointLight(point_lights[i], normalized_normal);	
	}
	
	for (int i = 0; i < num_spot_lights; ++i)
	{
		result += CalculateSpotLight(spot_lights[i], normalized_normal);
	}
	FragColor = vec4(result,1.0);
}

vec4 GetDiffuseColor(const Material mat, const vec2 texCoord)
{
	return texture(mat.diffuse_map, texCoord)*mat.diffuse_color;
}

vec4 GetSpecularColor(const Material mat, const vec2 texCoord)
{
	return texture(mat.specular_map, texCoord)*mat.specular_color;
}

vec3 GetOcclusionColor(const Material mat, const vec2 texCoord)
{
	return texture(mat.occlusion_map, texCoord).rgb * vec3(1.0,1.0,1.0);
}

vec3 GetEmissiveColor(const Material mat, const vec2 texCoord)
{
	return (texture(mat.emissive_map, texCoord)*mat.emissive_color).rgb;
}


vec3 CalculateDirectionalLight(const vec3 normalized_normal)
{

	vec3 light_dir   = normalize(-directional_light.direction );
	float diffuse    = max(0.0, dot(normalized_normal, light_dir));
	float specular   = 0.0;

	 vec3 view_pos    = transpose(mat3(matrices.proj))*(-matrices.proj[3].xyz);
     vec3 view_dir    = normalize(view_pos-position);
     vec3 reflect_dir = reflect(-light_dir, normalized_normal);
     float spec       = max(dot(view_dir, reflect_dir), 0.0);

	specular = pow(spec, material.shininess);
	

	vec4 diffuse_color  = GetDiffuseColor(material, texCoord);
	vec4 specular_color  = GetSpecularColor(material, texCoord);
	vec3 occlusion_color = GetOcclusionColor(material, texCoord);
	vec3 emissive_color  = GetEmissiveColor(material, texCoord);


	return directional_light.color * (
		emissive_color
		+ diffuse_color.rgb * (occlusion_color*material.k_ambient)
		+ diffuse_color.rgb * material.k_diffuse * diffuse
		+ specular_color.rgb * material.k_specular * specular
	);
}

vec3 CalculatePointLight(PointLight point_light, const vec3 normalized_normal)
{
	vec3 light_dir   = normalize(point_light.position - position);
	float diffuse    = max(0.0, dot(normalized_normal, light_dir));
	float specular   = 0.0;

 
     vec3 view_pos    = transpose(mat3(matrices.proj))*(-matrices.proj[3].xyz);
     vec3 view_dir    = normalize(view_pos-position);
     vec3 reflect_dir = reflect(-light_dir, normalized_normal);
     float spec       = max(dot(view_dir, reflect_dir), 0.0);

     specular = pow(spec, material.shininess);
      
  

	vec4 diffuse_color  = GetDiffuseColor(material, texCoord);
	vec4 specular_color  = GetSpecularColor(material, texCoord);
	vec3 occlusion_color = GetOcclusionColor(material, texCoord);
	vec3 emissive_color  = GetEmissiveColor(material, texCoord);
	
	float distance    = length(point_light.position - position);
	float attenuation = 1.0 / (point_light.constant + point_light.linear * distance + 
    		    point_light.quadratic * (distance * distance));    


	return point_light.color * (
		emissive_color
		+ diffuse_color.rgb * (occlusion_color*material.k_ambient)*attenuation
		+ diffuse_color.rgb *  material.k_diffuse * diffuse*attenuation
		+ specular_color.rgb * material.k_specular * specular*attenuation
	);

}


vec3 CalculateSpotLight(SpotLight spot_light, const vec3 normalized_normal)
{
	vec3 light_dir   = normalize(spot_light.position - position);
    float diffuse    = max(0.0, dot(normalized_normal, light_dir));
    float specular   = 0.0;

	
     vec3 view_pos    = transpose(mat3(matrices.proj))*(-matrices.proj[3].xyz);
     vec3 view_dir    = normalize(view_pos-position);
     vec3 reflect_dir = reflect(-light_dir, normalized_normal);
     float spec       = max(dot(view_dir, reflect_dir), 0.0);

     specular = pow(spec, material.shininess);
      

    vec4 diffuse_color  = GetDiffuseColor(material, texCoord);
	vec4 specular_color  = GetSpecularColor(material, texCoord);
	vec3 occlusion_color = GetOcclusionColor(material, texCoord);
	vec3 emissive_color  = GetEmissiveColor(material, texCoord);

    float theta = dot(light_dir, normalize(-spot_light.direction)); 
    float epsilon = (spot_light.cutOff - spot_light.outerCutOff);
    float intensity = clamp((theta - spot_light.outerCutOff) / epsilon, 0.0, 1.0);
    
    float distance    = length(spot_light.position - position);
    float attenuation = 1.0 / (spot_light.constant + spot_light.linear * distance + 
                spot_light.quadratic * (distance * distance));    

   return spot_light.color * (
        emissive_color
        + diffuse_color.rgb * (occlusion_color*material.k_ambient)*attenuation
        + diffuse_color.rgb * material.k_diffuse * diffuse*intensity*attenuation
        + specular_color.rgb * material.k_specular * specular*intensity*attenuation
    );

}
