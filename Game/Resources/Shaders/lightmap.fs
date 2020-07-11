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
const float gamma = 2.2;
const float ambient_light_strength = 0.1;

vec3 normal_from_texture;
vec3 liquid_normal_from_texture;

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
vec4 GetSpecularColor(const Material mat, const vec2 texCoord);
vec3 CalculateLightmap(const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color);
vec3 GetOcclusionColor(const Material mat, const vec2 texCoord);
vec3 GetEmissiveColor(const Material mat, const vec2 texCoord);

//MAPS
vec3 GetNormalMap(const Material mat, const vec2 texCoord);

vec3 NormalizedDiffuse(vec3 diffuse_color, vec3 frensel);

uniform float ambient_light_intensity;
uniform vec4 ambient_light_color;

void main()
{
	vec3 result = vec3(0);
	vec3 ambient = ambient_light_color.xyz* ambient_light_strength*ambient_light_intensity;
	//tiling
	vec2 tiling = vec2(material.tiling_x, material.tiling_y)*texCoord;

	//computation of colors
	vec4 diffuse_color  = GetDiffuseColor(material, tiling);
	vec4 specular_color  = GetSpecularColor(material, tiling);
	vec3 occlusion_color = GetOcclusionColor(material, tiling);
	vec3 emissive_color  = GetEmissiveColor(material, tiling);

	vec3 fragment_normal = normalize(normal);

	//Tangent space matrix
	vec3 T = normalize(vec3(matrices.model * vec4(vertex_tangent_fs,   0.0)));
	vec3 N = normalize(vec3(matrices.model * vec4(vertex_normal_fs,    0.0)));
	vec3 ortho_tangent = normalize(T-dot(T, N)*N); // Gram-Schmidt
	vec3 B = normalize(cross(N, ortho_tangent));
	mat3 TBN = mat3(T, B, N);

	#if NORMAL_MAP
		vec3 normal_from_texture = GetNormalMap(material, tiling);
		fragment_normal= normalize(TBN * normal_from_texture);
	#endif
	result += CalculateLightmap(fragment_normal, diffuse_color,  specular_color, occlusion_color,  emissive_color);
	result += emissive_color;
	result += diffuse_color.rgb * ambient * occlusion_color.rgb; //Ambient light
	FragColor = vec4(result,1.0);
	FragColor.rgb = pow(FragColor.rgb, vec3(1/gamma)); //Gamma Correction - The last operation of postprocess
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
	result.rgb = pow(result.rgb, vec3(2.2));
	return result;
}

vec4 GetSpecularColor(const Material mat, const vec2 texCoord)
{

	vec4 result = texture(mat.specular_map, texCoord)*mat.specular_color;
	result.rgb = pow(result.rgb, vec3(2.2));
	result.a *= mat.smoothness;
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

vec3 GetNormalMap(const Material mat, const vec2 texCoord)
{
	return normalize(texture(mat.normal_map, texCoord).rgb*2.0-1.0);
}

vec3 NormalizedDiffuse(vec3 diffuse_color, vec3 frensel)
{
	return (1-frensel)*diffuse_color/PI;
}

vec3 CalculateLightmap(const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color)
{

		vec3 light_dir   =vec3(1.0f,1.0f,1.0f);
	vec3 half_dir 	 = normalize(light_dir + view_dir);
	float ND = max(0.0, dot(normalized_normal, light_dir));

	//----Specular calculations----
	float shininess = 7*specular_color.a + 1;
	shininess *= shininess;
	float spec = pow(max(dot(normalized_normal, half_dir), 0.0), shininess);
	vec3 fresnel =  specular_color.rgb + (1-specular_color.rgb)* pow((1.0-ND),5);
	vec3 specular =(spec * (shininess+8)/8*PI) *  fresnel;

	return texture(material.light_map, texCoord).rgb * (NormalizedDiffuse(diffuse_color.rgb, fresnel) + specular)* ND;

}

