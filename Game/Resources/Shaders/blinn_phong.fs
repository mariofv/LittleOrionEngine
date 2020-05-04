#version 430 core
#define PI 3.14159

//General variables
in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec3 tangent;

//Tangent - Normal mapping variables
in mat3 TBN;




out vec4 FragColor;

//constants
const float gamma = 2.2;

struct Material
{
	sampler2D diffuse_map;
	vec4 diffuse_color;
	float k_diffuse;
	sampler2D specular_map;
	vec4 specular_color;
	float k_specular;

	sampler2D occlusion_map; 
	float k_ambient;
	sampler2D emissive_map;
	vec4 emissive_color;
	sampler2D normal_map;

	float roughness;
	float metalness;
	float transparency;
	float tiling_x;
	float tiling_y;
	bool use_normal_map;
	bool use_specular_map;
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

struct PointLight
{
	vec3 color;
	vec3 position;

	float constant;
    float linear;
    float quadratic;
};

uniform int num_spot_lights;
uniform SpotLight spot_lights[10];

uniform int num_point_lights;
uniform PointLight point_lights[10];


//COLOR TEXTURES
vec4 GetDiffuseColor(const Material mat, const vec2 texCoord);
vec4 GetSpecularColor(const Material mat, const vec2 texCoord);
vec3 GetOcclusionColor(const Material mat, const vec2 texCoord);
vec3 GetEmissiveColor(const Material mat, const vec2 texCoord);

//MAPS
vec3 GetNormalMap(const Material mat, const vec2 texCoord);

//TYPE OF LIGHTS
vec3 CalculateDirectionalLight(const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color);
vec3 CalculateSpotLight(SpotLight spot_light, const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color);
vec3 CalculatePointLight(PointLight point_light, const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color);

//COMPUTE NORMALIZED LIGHTS
vec3 NormalizedDiffuse(vec3 diffuse_color, vec3 specular_color);
float NormalizedSpecular(vec3 normal, vec3 half_dir);


void main()
{

	vec3 result = vec3(0);

	//tiling
	vec2 tiling = vec2(material.tiling_x, material.tiling_y)*texCoord; 

	//computation of colors
	vec4 diffuse_color  = GetDiffuseColor(material, tiling);
	vec4 specular_color  = GetSpecularColor(material, tiling);
	vec3 occlusion_color = GetOcclusionColor(material, tiling);
	vec3 emissive_color  = GetEmissiveColor(material, tiling);

	if(material.use_normal_map)
	{
		vec3 normal_from_texture = GetNormalMap(material, tiling);

		vec3 fragment_normal = normalize(TBN * normal_from_texture);

		for (int i = 0; i < directional_light.num_directional_lights; ++i)
		{
			result += CalculateDirectionalLight(fragment_normal, diffuse_color,  specular_color, occlusion_color,  emissive_color);

		}

		for (int i = 0; i < num_spot_lights; ++i)
		{
			result += CalculateSpotLight(spot_lights[i], fragment_normal, diffuse_color,  specular_color, occlusion_color,  emissive_color);
		}

		for (int i = 0; i < num_point_lights; ++i)
		{
			result += CalculatePointLight(point_lights[i], fragment_normal, diffuse_color,  specular_color, occlusion_color,  emissive_color);	
		}
	}

	else
	{
		for (int i = 0; i < directional_light.num_directional_lights; ++i)
		{
			result += CalculateDirectionalLight(normal, diffuse_color,  specular_color, occlusion_color,  emissive_color);

		}

		for (int i = 0; i < num_spot_lights; ++i)
		{
			result += CalculateSpotLight(spot_lights[i], normal, diffuse_color,  specular_color, occlusion_color,  emissive_color);
		}

		for (int i = 0; i < num_point_lights; ++i)
		{
			result += CalculatePointLight(point_lights[i], normal, diffuse_color,  specular_color, occlusion_color,  emissive_color);	
		}
	}

	//Ambient light
	result +=  diffuse_color.rgb * (occlusion_color*material.k_ambient);

	FragColor = vec4(result,1.0);
	
	//Gamma Correction - The last operation of postprocess
	FragColor.rgb = pow(FragColor.rgb, vec3(1/gamma));


	FragColor.a=material.transparency;
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
	return texture(mat.occlusion_map, texCoord).rgb;
}

vec3 GetEmissiveColor(const Material mat, const vec2 texCoord)
{
	return (texture(mat.emissive_map, texCoord)*mat.emissive_color).rgb;
}

vec3 GetNormalMap(const Material mat, const vec2 texCoord)
{
	return normalize(texture(mat.normal_map, texCoord).rgb*2.0-1.0);
}

vec3 CalculateDirectionalLight(const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color)
{
	
	vec3 view_pos    = transpose(mat3(matrices.view)) * (-matrices.view[3].xyz);
	vec3 view_dir    = normalize(view_pos - position);
	vec3 light_dir   = normalize(-directional_light.direction );
	float specular   = 0.0;	
	vec3 half_dir 	 = normalize(light_dir + view_dir);

	if(material.k_specular > 0.0 && material.specular_color.w > 0.0)
	{	
		specular = NormalizedSpecular(normalized_normal, half_dir);
	}

	return directional_light.color * (
		emissive_color
		+ NormalizedDiffuse(diffuse_color.rgb, specular_color.rgb) * 1/PI
		+ specular_color.rgb * specular
	) * max(0.0, dot(normalized_normal, light_dir));
	//Last multiplication added as a recommendation
}

vec3 CalculateSpotLight(SpotLight spot_light, const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color)
{
	vec3 view_pos    = transpose(mat3(matrices.view)) * (-matrices.view[3].xyz);
	vec3 view_dir    = normalize(view_pos - position);
	vec3 light_dir   = normalize(spot_light.position - position);
    float specular   = 0.0;
	vec3 half_dir 	 = normalize(light_dir + view_dir);

	if(material.k_specular > 0.0 && material.specular_color.w > 0.0)
	{	
		specular = NormalizedSpecular(normalized_normal, half_dir);
	}

    float theta = dot(light_dir, normalize(-spot_light.direction)); 
    float epsilon = (spot_light.cutOff - spot_light.outerCutOff);
    float intensity = clamp((theta - spot_light.outerCutOff) / epsilon, 0.0, 1.0);
    
    float distance    = length(spot_light.position - position);
    float attenuation = 1.0 / (spot_light.constant + spot_light.linear * distance + 
                spot_light.quadratic * (distance * distance));    

   return spot_light.color * (
        emissive_color
        + NormalizedDiffuse(diffuse_color.rgb, specular_color.rgb) * 1/PI *intensity*attenuation
        + specular_color.rgb * specular *intensity*attenuation
    )* max(0.0, dot(normalized_normal, light_dir));

}

vec3 CalculatePointLight(PointLight point_light, const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color)
{

	vec3 view_pos    = transpose(mat3(matrices.view)) * (-matrices.view[3].xyz);
	vec3 view_dir    = normalize(view_pos - position);
	vec3 light_dir   = normalize(point_light.position - position);
	float specular   = 0.0;
	vec3 half_dir 	 = normalize(light_dir + view_dir);

   if(material.k_specular > 0.0 && material.specular_color.w > 0.0)
	{	
		specular = NormalizedSpecular(normalized_normal, half_dir);
	}

	float distance    = length(point_light.position - position);
	float attenuation = 1.0 / (point_light.constant + point_light.linear * distance + 
    		    point_light.quadratic * (distance));    

	return point_light.color * (
		emissive_color
		+ NormalizedDiffuse(diffuse_color.rgb, specular_color.rgb) * 1/PI * attenuation
		+ specular_color.rgb * specular * attenuation
	)* max(0.0, dot(normalized_normal, light_dir));

}

vec3 NormalizedDiffuse(vec3 diffuse_color, vec3 specular_color)
{
	return 	(1-specular_color)*diffuse_color; //The more specular, the less diffuse
}

float NormalizedSpecular(vec3 normal, vec3 half_dir) // Old refference: http://www.farbrausch.de/~fg/stuff/phong.pdf
{
	
	float shininess = pow(7*material.specular_color.w + 1, 2); 

	float spec = pow(max(dot(normal, half_dir), 0.0), shininess);

	float normalization_factor = (material.specular_color.w + 8)/8;

	return pow(spec, material.specular_color.w) * normalization_factor;
}




