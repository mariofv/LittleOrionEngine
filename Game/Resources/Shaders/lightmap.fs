#version 430 core
#define PI 3.14159

//General variables
in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec2 texCoordLightmap;
in vec3 tangent;

//Without tangent transform
in vec3 view_pos;
in vec3 view_dir;

//Tangent - Normal mapping variables
in mat3 TBN;
in vec3 t_view_pos;
in vec3 t_frag_pos;



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
	sampler2D light_map;

	float roughness;
	float metalness;
	float transparency;
	float tiling_x;
	float tiling_y;
	bool use_normal_map;
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


//COLOR TEXTURES
vec4 GetDiffuseColor(const Material mat, const vec2 texCoord);
vec4 GetSpecularColor(const Material mat, const vec2 texCoord);
vec3 GetLightMapColor(const Material mat, const vec2 texCoordLightmap);
vec3 GetOcclusionColor(const Material mat, const vec2 texCoord);
vec3 GetEmissiveColor(const Material mat, const vec2 texCoord);

//MAPS
vec3 GetNormalMap(const Material mat, const vec2 texCoord);

//TYPE OF LIGHTS
vec3 CalculateLightmap(const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color);
//COMPUTE NORMALIZED LIGHTS
vec3 ComputeDiffuseColor(vec3 diffuse_color, vec3 specular_color);
float ComputeSpecularLight(vec3 normal, vec3 half_dir);


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

		result += CalculateLightmap(fragment_normal, diffuse_color,  specular_color, occlusion_color,  emissive_color);
	}

	else
	{
		result += CalculateLightmap(normal, diffuse_color,  specular_color, occlusion_color,  emissive_color);
	}

	result += emissive_color;
	//FragColor = vec4(vec3(normalize(tangent)),1.0);
	FragColor = vec4(result,1.0);

	//Gamma Correction
	FragColor.rgb = pow(FragColor.rgb, vec3(1/gamma));
	FragColor.a=material.transparency;
}

vec4 GetDiffuseColor(const Material mat, const vec2 texCoord)
{

	vec4 result = texture(mat.diffuse_map, texCoord)*mat.diffuse_color;
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

vec3 GetNormalMap(const Material mat, const vec2 texCoord)
{
	return texture(mat.normal_map, texCoord).rgb*2.0-1.0;
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
		specular = ComputeSpecularLight(normalized_normal, half_dir);
	}

	return lightmap_color * (

		+ diffuse_color.rgb * (occlusion_color*material.k_ambient)
		+ ComputeDiffuseColor(diffuse_color.rgb, specular_color.rgb) * 1/PI * diffuse
		+ specular_color.rgb * specular
	);
}

vec3 ComputeDiffuseColor(vec3 diffuse_color, vec3 specular_color)
{
	return 	(1-specular_color)*diffuse_color;
}

float ComputeSpecularLight(vec3 normal, vec3 half_dir) // Refference: http://www.farbrausch.de/~fg/stuff/phong.pdf
{
	float spec = pow(max(dot(normal, half_dir), 0.0), 64.0);

	float normalization_denom = (material.specular_color.w + 2) * (material.specular_color.w + 4);
	float normalization_nom = 8*PI *(pow(2, -material.specular_color.w/2) + material.specular_color.w);

	return pow(spec, material.specular_color.w) * (normalization_denom/normalization_nom);
}
