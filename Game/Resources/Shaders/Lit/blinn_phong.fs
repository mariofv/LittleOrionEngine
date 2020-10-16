//////////////////////////////////
///////     CONSTANTS    /////////
//////////////////////////////////

#define PI 3.14159

float gamma = 2.2;
const float ambient_light_strength = 0.1;

//////////////////////////////////
////////     STRUCTS    //////////
//////////////////////////////////

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
	float emissive_intensity;

	sampler2D normal_map;
	sampler2D light_map;

	float roughness;
	float metalness;
	float transparency;
	vec2 tiling;

	sampler2D liquid_map;
	vec2 liquid_horizontal_normals_tiling;
	vec2 liquid_vertical_normals_tiling;

	sampler2D dissolved_diffuse;
	sampler2D dissolved_emissive;
	sampler2D dissolved_noise;
	float dissolve_progress;

	vec4 final_added_color;
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

struct PointLight
{
	vec3 color;
	vec3 position;

	float constant;
  float linear;
  float quadratic;
};

//////////////////////////////////
////////     LAYOUTS    //////////
//////////////////////////////////

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

//////////////////////////////////
///////     UNIFORMS    //////////
//////////////////////////////////

uniform Material material;

// LIGHTS
uniform int num_spot_lights;
uniform SpotLight spot_lights[10];

uniform int num_point_lights;
uniform PointLight point_lights[10];

// SHADOWS
uniform sampler2D full_depth_map;
uniform sampler2D close_depth_map;
uniform sampler2D mid_depth_map;
uniform sampler2D far_depth_map;

uniform float ambient_light_intensity;
uniform vec4 ambient_light_color;

//////////////////////////////////
///////     FUNCTIONS    /////////
//////////////////////////////////

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

vec3 GetLiquidNormal(const Material material);
vec3 NormalizedDiffuse(vec3 diffuse_color, vec3 frensel);

//SHADOW MAPS
float ShadowCalculation();
float PercentageCloserFiltering(vec3 normalized_position, sampler2D depth_map);
vec3 CascadeVisualization();
vec3 NormalizePosition(vec4 position);
bool InsideUVRange(vec2 uv);

//////////////////////////////////
///////     PARAMETERS    ////////
//////////////////////////////////

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

in vec4 position_near_depth_space;
in vec4 position_mid_depth_space;
in vec4 position_far_depth_space;
in vec4 position_full_depth_space;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

//////////////////////////////////
///////     DEFINTIONS    ////////
//////////////////////////////////

void main()
{
	vec3 result = vec3(0);
	vec3 ambient = ambient_light_color.xyz * ambient_light_strength * ambient_light_intensity;
	//tiling
	vec2 tiling = material.tiling * texCoord;

	//TODO->change it to liquid maps and not hardcoded
#if ENABLE_LIQUID_PROPERTIES
		tiling -= vec2(0, material.liquid_vertical_normals_tiling.y);
#endif

	vec3 fragment_normal = normalize(normal);

	//Tangent space matrix
	vec3 T = normalize(vec3(matrices.model * vec4(vertex_tangent_fs, 0.0)));
	vec3 N = normalize(vec3(matrices.model * vec4(vertex_normal_fs, 0.0)));
	vec3 ortho_tangent = normalize(T-dot(T, N)*N); // Gram-Schmidt
	vec3 B = normalize(cross(N, ortho_tangent));
	mat3 TBN = mat3(T, B, N);

	#if NORMAL_MAP
		vec3 normal_from_texture = GetNormalMap(material, tiling);
		fragment_normal= normalize(TBN * normal_from_texture);
	#endif

	#if LIQUID_MAP
		vec3 liquid_normal = GetLiquidNormal(material);
		fragment_normal = normalize(TBN * liquid_normal);
	#endif

	//computation of colors
	vec4 diffuse_color  = GetDiffuseColor(material, tiling);
	vec4 specular_color  = GetSpecularColor(material, tiling);
	vec3 occlusion_color = GetOcclusionColor(material, tiling);
	vec3 emissive_color  = GetEmissiveColor(material, tiling);

 	float lit_fragment = ShadowCalculation();

	result += diffuse_color.rgb * ambient * occlusion_color.rgb; //Ambient light

#if ENABLE_LIGHT_MAP
	result += diffuse_color.rgb * GetLightMapColor(material, texCoordLightmap).rgb * lit_fragment;
#else

	for (int i = 0; i < directional_light.num_directional_lights; ++i)
	{
		result += CalculateDirectionalLight(fragment_normal, diffuse_color,  specular_color, occlusion_color,  emissive_color) * lit_fragment;
	}
#endif

	for (int i = 0; i < num_spot_lights; ++i)
	{
		result += CalculateSpotLight(spot_lights[i], fragment_normal, diffuse_color,  specular_color, occlusion_color,  emissive_color);
	}

	for (int i = 0; i < num_point_lights; ++i)
	{
		result += CalculatePointLight(point_lights[i], fragment_normal, diffuse_color,  specular_color, occlusion_color,  emissive_color);
	}

	result += emissive_color;

	FragColor = vec4(result,1.0) + material.final_added_color;

#if	ENABLE_CASCADE_VISUALIZATION
	FragColor.rgb = CascadeVisualization();
#endif

	FragColor.a = material.transparency;

	float brightness = dot(result, vec3(0.2, 0.6, 0.0));
	if (brightness > 1.0)
	{
  		BrightColor = vec4(result, 1.0);
	}
	else
	{
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}

vec4 GetDiffuseColor(const Material mat, const vec2 texCoord)
{
	vec4 result;
	#if ENABLE_DISSOLVING_PROPERTIES
			float mapped_noise = texture(mat.dissolved_noise, texCoord).x;
			if (mapped_noise > mat.dissolve_progress)
			{
				result = texture(mat.diffuse_map, texCoord);
			}
			else
			{
				result = texture(mat.dissolved_diffuse, texCoord);
			}
	#else
			result = texture(mat.diffuse_map, texCoord);
	#endif

	result = result * mat.diffuse_color;
	//alpha testing
	if(result.a <0.1)
	{
		discard;
	}
	result.rgb = pow(result.rgb, vec3(2.2));
	return result;
}

vec3 GetLightMapColor(const Material mat, const vec2 texCoord)
{
	return pow(texture(mat.light_map, texCoord).rgb, vec3(2.2));
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
	vec4 result;
	#if ENABLE_DISSOLVING_PROPERTIES
			float mapped_noise = texture(mat.dissolved_noise, texCoord).x;
			if (mapped_noise > mat.dissolve_progress)
			{
				result = texture(mat.emissive_map, texCoord);
			}
			else
			{
				result = texture(mat.dissolved_emissive, texCoord);
			}
	#else
			result = texture(mat.emissive_map, texCoord);
	#endif

	result = result * mat.emissive_color * mat.emissive_intensity;
	result.rgb = pow(result.rgb, vec3(2.2));
	return result.rgb;
}

vec3 GetNormalMap(const Material mat, const vec2 texCoord)
{
	vec3 normal_map = texture(mat.normal_map, texCoord).rgb*2.0-1.0 ;
	normal_map.g = -normal_map.g;
	return normalize(normal_map.rgb);
}
vec3 GetLiquidMap(const Material mat, const vec2 texCoord)
{
	return texture(mat.liquid_map, texCoord).rgb*2.0-1.0;
}

vec3 CalculateDirectionalLight(const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color)
{

		vec3 light_dir   = normalize(-directional_light.direction);
		vec3 half_dir 	 = normalize(light_dir + view_dir);
		float ND = max(0.0, dot(normalized_normal, light_dir));

		//----Specular calculations----
		float shininess = 7*specular_color.a + 1;
		shininess *= shininess;
		float spec = pow(max(dot(normalized_normal, half_dir), 0.0), shininess);
		vec3 fresnel =  specular_color.rgb + (1-specular_color.rgb)* pow((1.0-ND),5);
		vec3 specular =(spec * (shininess+8)/8*PI) *  fresnel;

		return directional_light.color * (NormalizedDiffuse(diffuse_color.rgb, fresnel) + specular) * ND;
}

vec3 CalculateSpotLight(SpotLight spot_light, const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color)
{
	vec3 light_dir   = normalize(spot_light.position - position);
	vec3 half_dir 	 = normalize(light_dir + view_dir);
	float ND = max(0.0, dot(normalized_normal, light_dir));

	//----Specular calculations----
	float shininess = 7*specular_color.a + 1;
	shininess *= shininess;
	float spec = pow(max(dot(normalized_normal, half_dir), 0.0), shininess);
	vec3 fresnel =  specular_color.rgb + (1-specular_color.rgb)* pow((1.0-ND),5);
	vec3 specular =(spec * (shininess+8)/8*PI) *  fresnel;

	//----Softness and atenuation---
    float theta = dot(light_dir, normalize(-spot_light.direction));
    float epsilon = (spot_light.cutOff - spot_light.outerCutOff);
    float intensity = clamp((theta - spot_light.outerCutOff) / epsilon, 0.0, 1.0);
    float distance    = length(spot_light.position - position);
    float attenuation = 1.0 / (spot_light.constant + spot_light.linear * distance +
                spot_light.quadratic * (distance * distance));

   return spot_light.color * (
         NormalizedDiffuse(diffuse_color.rgb, fresnel) *intensity*attenuation
        +  specular *intensity*attenuation
    )*ND;

}

vec3 CalculatePointLight(PointLight point_light, const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color)
{
	vec3 light_dir   = normalize(point_light.position - position);
	vec3 half_dir 	 = normalize(light_dir + view_dir);
	float ND = max(0.0, dot(normalized_normal, light_dir));

//----Specular calculations----
	float shininess = 7*specular_color.a+ 1;
	shininess *= shininess;
    float spec = pow(max(dot(normalized_normal, half_dir), 0.0), shininess);
	vec3 fresnel =  specular_color.rgb + (1-specular_color.rgb)* pow((1.0-ND),5);
    vec3 specular =(spec * (shininess+8)/8*PI) *  fresnel;

  	//----Softness and atenuation---
	float distance    = length(point_light.position - position);
	float attenuation = 1.0 / (point_light.constant + point_light.linear * distance +
    		    point_light.quadratic * (distance));

	return point_light.color * (
		 NormalizedDiffuse(diffuse_color.rgb, fresnel)  * attenuation
		+ specular * attenuation
	)*ND;

}

vec3 NormalizedDiffuse(vec3 diffuse_color, vec3 frensel)
{
	return (1-frensel)*diffuse_color/PI;
}

float ShadowCalculation()
{
#if	!ENABLE_RECEIVE_SHADOWS
	return 1.0;
#endif

#if	!ENABLE_CASCADE_MAPPING
	vec3 normalized_position_full_depth_space = NormalizePosition(position_full_depth_space);
	if (normalized_position_full_depth_space.z > 1.0)
	{
		return 1.0;
	}
	return PercentageCloserFiltering(normalized_position_full_depth_space, full_depth_map);

#else
	vec3 normalized_position_near_depth_space = NormalizePosition(position_near_depth_space);
	if (InsideUVRange(normalized_position_near_depth_space.xy) && normalized_position_near_depth_space.z <= 1.0)
	{
		return PercentageCloserFiltering(normalized_position_near_depth_space, close_depth_map);
	}

	vec3 normalized_position_mid_depth_space = NormalizePosition(position_mid_depth_space);
	if (InsideUVRange(normalized_position_mid_depth_space.xy)	&& normalized_position_mid_depth_space.z <= 1.0)
	{
		return PercentageCloserFiltering(normalized_position_mid_depth_space, mid_depth_map);
	}

	vec3 normalized_position_far_depth_space = NormalizePosition(position_far_depth_space);
	if (InsideUVRange(normalized_position_far_depth_space.xy) && normalized_position_far_depth_space.z <= 1.0)
	{
		return PercentageCloserFiltering(normalized_position_far_depth_space, far_depth_map);
	}

	return 1.0;

#endif
}

float PercentageCloserFiltering(vec3 normalized_position, sampler2D depth_map)
{
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(depth_map, 0);
	for(int x = -1; x <= 1; ++x)
	{
	    for(int y = -1; y <= 1; ++y)
	    {
	        float pcf_depth = texture(depth_map, normalized_position.xy + vec2(x, y) * texelSize).r;
	        shadow += normalized_position.z > pcf_depth ? 1.0 : 0.0;
	    }
	}
	shadow /= 9.0;
	return (1 - shadow);
}

vec3 CascadeVisualization()
{
	//Light frustums
	vec3 normalized_position_near_depth_space = NormalizePosition(position_near_depth_space);
	if(InsideUVRange(normalized_position_near_depth_space.xy))
	{
		return vec3(1.0, 0.0, 0.0);
	}

	vec3 normalized_position_mid_depth_space = NormalizePosition(position_mid_depth_space);
	if(InsideUVRange(normalized_position_mid_depth_space.xy))
	{
		return vec3(0.0, 1.0, 0.0);
	}

	vec3 normalized_position_far_depth_space = NormalizePosition(position_far_depth_space);
	if(InsideUVRange(normalized_position_far_depth_space.xy))
	{
		return vec3(0.0, 0.0, 1.0);
	}

	return vec3(1.0, 1.0, 1.0);
}

vec3 NormalizePosition(vec4 position)
{
	return (position.xyz / position.w) * 0.5 + 0.5;
}

bool InsideUVRange(vec2 uv)
{
	return uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0;
}

vec3 GetLiquidNormal(const Material material)
{
	vec2 liquid_horizontal_normal_coord = material.liquid_horizontal_normals_tiling + material.tiling;
	vec2 liquid_vertical_normal_coord = material.liquid_vertical_normals_tiling + material.tiling;
	vec3 liquid_horizontal_normal = GetLiquidMap(material, liquid_horizontal_normal_coord);
	vec3 liquid_vertical_normal = GetLiquidMap(material, liquid_vertical_normal_coord);
	return  mix(liquid_horizontal_normal, liquid_vertical_normal, 0.5);
}
