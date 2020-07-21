#define PI 3.14159

//General variables
in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec2 texCoordLightmap;
in vec3 view_dir;
in vec3 view_pos;
in vec3 vertex_normal_fs;
in vec3 vertex_tangent_fs;

//Tangent - Normal mapping variables
in mat3 TBN;

out vec4 FragColor;

//constants

const float ambient_light_strength = 0.1;

struct Material
{
	sampler2D diffuse_map;
	vec4 diffuse_color;
	sampler2D specular_map;
	vec4 specular_color;
	float smoothness;

	sampler2D occlusion_map;
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
};
uniform Material material;

layout (std140) uniform Matrices
{
	mat4 model;
	mat4 proj;
	mat4 view;
} matrices;


//COLOR TEXTURES
vec4 GetDiffuseColor(const Material mat, const vec2 texCoord);
vec3 GetOcclusionColor(const Material mat, const vec2 texCoord);
vec3 GetEmissiveColor(const Material mat, const vec2 texCoord);

//MAPS

uniform float ambient_light_intensity;
uniform vec4 ambient_light_color;

void main()
{
	vec3 result = vec3(0);
	vec3 ambient = ambient_light_color.xyz* ambient_light_strength*ambient_light_intensity;
	//tiling

	//computation of colors
	vec4 diffuse_color  = GetDiffuseColor(material, texCoord);
	vec3 occlusion_color = GetOcclusionColor(material, texCoord);
	vec3 emissive_color  = GetEmissiveColor(material, texCoord);


	//result += emissive_color;
	result +=diffuse_color.rgb + ambient + emissive_color;
	result *= texture(material.light_map, texCoordLightmap).rgb;
	FragColor = vec4(result,diffuse_color.a);
}

vec4 GetDiffuseColor(const Material mat, const vec2 texCoord)
{
	vec4 result = texture(mat.diffuse_map, texCoord)*mat.diffuse_color;
	if(result.a <0.5)
	{
		discard;
	}
	return result;
}

vec3 GetOcclusionColor(const Material mat, const vec2 texCoord)
{
	return texture(mat.occlusion_map, texCoord).rgb;
}

vec3 GetEmissiveColor(const Material mat, const vec2 texCoord)
{
	return (texture(mat.emissive_map, texCoord)*mat.emissive_color).rgb;
}




