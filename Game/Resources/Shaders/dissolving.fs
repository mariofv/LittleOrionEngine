#version 430 core
#define PI 3.14159

//General variables
in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec2 texCoordLightmap;
in vec3 tangent;

in vec3 view_dir;
in vec3 view_pos;

//Tangent - Normal mapping variables
in mat3 TBN;

out vec4 FragColor;

//constants
const float gamma = 2.2;
const float ambient_light_strength = 0.1;

vec3 normal_from_texture;
vec3 liquid_normal_from_texture;

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
	sampler2D light_map;

	float roughness;
	float metalness;
	float transparency;
	float tiling_x;
	float tiling_y;

	sampler2D liquid_map;
	float tiling_liquid_x_x;
	float tiling_liquid_x_y;
	float tiling_liquid_y_x;
	float tiling_liquid_y_y;
	bool use_liquid_map;

	sampler2D dissolved_diffuse;
	sampler2D dissolved_noise;
	float dissolve_progress;
	bool use_noise_map;

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

uniform int use_light_map;


//COLOR TEXTURES
vec4 GetDiffuseColor(const Material mat, const vec2 texCoord);
vec4 GetSpecularColor(const Material mat, const vec2 texCoord);
vec3 GetLightMapColor(const Material mat, const vec2 texCoordLightmap);
vec3 GetOcclusionColor(const Material mat, const vec2 texCoord);
vec3 GetEmissiveColor(const Material mat, const vec2 texCoord);

//MAPS
vec3 GetNormalMap(const Material mat, const vec2 texCoord);
vec3 GetLiquidMap(const Material mat, const vec2 texCoord);

//TYPE OF LIGHTS
vec3 CalculateDirectionalLight(const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color);
vec3 CalculateSpotLight(SpotLight spot_light, const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color);
vec3 CalculatePointLight(PointLight point_light, const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color);
vec3 CalculateLightmap(const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color);

vec3 CalculateNormalMapAndLiquid(const Material material, const vec2 texCoord);

vec3 NormalizedDiffuse(vec3 diffuse_color, vec3 specular_color);
float NormalizedSpecular(vec3 normal, vec3 half_dir);

uniform float ambient_light_intensity;
uniform vec4 ambient_light_color;


void main()
{


	vec3 result = vec3(0);
	vec3 ambient = ambient_light_color.xyz* ambient_light_strength*ambient_light_intensity;
	vec2 tiling = texCoord;


	//computation of colors
	vec4 diffuse_color  = GetDiffuseColor(material, tiling);
	vec4 specular_color  = GetSpecularColor(material, tiling);
	vec3 occlusion_color = GetOcclusionColor(material, tiling);
	vec3 emissive_color  = GetEmissiveColor(material, tiling);

	vec3 fragment_normal = normalize(normal);
	result += CalculateLightmap(fragment_normal, diffuse_color,  specular_color, occlusion_color,  emissive_color);
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

	result += emissive_color;
	result += diffuse_color.rgb * ambient; //Ambient light

	FragColor = vec4(result,1.0);
	FragColor.rgb = pow(FragColor.rgb, vec3(1/gamma)); //Gamma Correction - The last operation of postprocess
	FragColor.a=material.transparency;
}

vec4 GetDiffuseColor(const Material mat, const vec2 texCoord)
{
	vec4 result;

	if (mat.use_noise_map)
	{
		float mapped_noise = 1 - texture(mat.dissolved_noise, texCoord).x;
		if (mapped_noise < mat.dissolve_progress)
		{
			result = texture(mat.diffuse_map, texCoord);
		}
		else
		{
			result = texture(mat.dissolved_diffuse, texCoord);
		}
	}
	else
	{
		result = texture(mat.diffuse_map, texCoord);
	}

	result = result * mat.diffuse_color;
	//alpha testing
	if(result.a <0.1)
	{
		discard;
	}
	return result;
}

vec3 GetLightMapColor(const Material mat, const vec2 texCoord)
{
	return texture(mat.light_map, texCoord).rgb;
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

vec3 GetLiquidMap(const Material mat, const vec2 texCoord)
{
	return texture(mat.liquid_map, texCoord).rgb*2.0-1.0;
}

vec3 CalculateDirectionalLight(const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color)
{

	vec3 view_pos    = transpose(mat3(matrices.view)) * (-matrices.view[3].xyz);
	vec3 view_dir    = normalize(view_pos - position);
	vec3 light_dir   = normalize(-directional_light.direction );
	vec3 half_dir 	 = normalize(light_dir + view_dir);


	float shadow;

	float specular = NormalizedSpecular(normalized_normal, half_dir);


		return
			directional_light.color
			 * (
				 emissive_color
				 + NormalizedDiffuse(diffuse_color.rgb, specular_color.rgb)
				 + specular_color.rgb * specular
			)
			* max(0.0, dot(normalized_normal, light_dir));
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
	if(material.use_specular_map)
	{
		return 	(1-specular_color)*diffuse_color * material.k_diffuse * 1/PI; //The more specular, the less diffuse
	}

	else
	{
		return 	diffuse_color * material.k_diffuse;
	}
}

float NormalizedSpecular(vec3 normal, vec3 half_dir) // Old refference: http://www.farbrausch.de/~fg/stuff/phong.pdf
{

	float shininess = pow(7*material.specular_color.w + 1, 2);
	float spec = pow(max(dot(normal, half_dir), 0.0), shininess);
	float normalization_factor = (spec + 8)/8;

	return spec* normalization_factor;
}

vec3 CalculateLightmap(const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color)
{

	vec3 lightmap_color  = GetLightMapColor(material, texCoordLightmap);

	vec3 light_dir   = vec3(1.0,1.0,1.0);
	float diffuse    = max(0.0, dot(normalized_normal, light_dir));
	float specular   = 0.0;

	vec3 half_dir = normalize(light_dir + view_dir);

	if(diffuse > 0.0 && material.k_specular > 0.0 && material.specular_color.w > 0.0)
	{
		specular = NormalizedSpecular(normalized_normal, half_dir);
	}

	return use_light_map * lightmap_color  * (

		+ diffuse_color.rgb * (occlusion_color*material.k_ambient)
		+ NormalizedDiffuse(diffuse_color.rgb, specular_color.rgb) * 1/PI * diffuse
		+ specular_color.rgb * specular
	);
}
