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

//SHADOW MAPS
float ShadowCalculation();
uniform bool render_shadows;

in vec4 close_pos_from_light;
in vec4 mid_pos_from_light;
in vec4 far_pos_from_light;

//perspective cams
//in vec4 pos_from_main_camera;
//in vec4 pos_from_close_camera;
//in vec4 pos_from_mid_camera;

uniform float main_cam_far_plane;

vec3 FrustumsCheck();

uniform sampler2DShadow close_depth_map;
uniform sampler2DShadow mid_depth_map;
uniform sampler2DShadow far_depth_map;

in float distance_to_camera;
uniform float far_plane;


void main()
{
	

	vec3 result = vec3(0);
	vec3 ambient = ambient_light_color.xyz* ambient_light_strength*ambient_light_intensity;
	//tiling
	vec2 tiling = vec2(material.tiling_x, material.tiling_y)*texCoord;
	//TODO->change it to liquid maps and not hardcoded
	if(material.use_liquid_map)
	{
		tiling += vec2(material.tiling_liquid_x_x, material.tiling_liquid_x_y);
	}

	//computation of colors
	vec4 diffuse_color  = GetDiffuseColor(material, tiling);
	vec4 specular_color  = GetSpecularColor(material, tiling);
	vec3 occlusion_color = GetOcclusionColor(material, tiling);
	vec3 emissive_color  = GetEmissiveColor(material, tiling);

	vec3 fragment_normal = normalize(normal);
	//fragment_normal = CalculateNormalMapAndLiquid(material, tiling); TODO change it to liquid maps
	if(material.use_normal_map)	
	{	
		vec3 normal_from_texture = GetNormalMap(material, tiling);	
		fragment_normal= normalize(TBN * normal_from_texture);	
	}
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
	//result += FrustumsCheck();
	//FragColor = vec4(vec3(normalize(tangent)),1.0);
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
	return normalize(texture(mat.normal_map, texCoord).rgb*2.0-1.0);
}
vec3 GetLiquidMap(const Material mat, const vec2 texCoord)
{
	return texture(mat.liquid_map, texCoord).rgb*2.0-1.0;
}
vec3 CalculateNormalMapAndLiquid(const Material material, const vec2 tiling)
{
	vec3 result;
	//if(material.use_liquid_map && !material.use_normal_map )
	//{
	//	vec2 tiling_nm_x = vec2(material.tiling_liquid_x_x, material.tiling_liquid_x_y)+tiling;
	//	vec2 tiling_nm_y = vec2(material.tiling_liquid_y_x, material.tiling_liquid_y_y)+tiling;
	//	vec3 normal_texture_x = GetLiquidMap(material, tiling_nm_x);
	//	vec3 normal_texture_y = GetLiquidMap(material, tiling_nm_y);
	//	liquid_normal_from_texture =  mix(normal_texture_x, normal_texture_y, 0.5);
	//	result = normalize(TBN * liquid_normal_from_texture);
	//}
	//else if(material.use_normal_map && !material.use_liquid_map)
	//{
	//	normal_from_texture = GetNormalMap(material, tiling);
	//	result = normalize(TBN * normal_from_texture);
	//}
	//else if(material.use_normal_map && material.use_liquid_map)
	//{
	//	vec2 tiling_nm_x = vec2(material.tiling_liquid_x_x, material.tiling_liquid_x_y)+tiling;
	//	vec2 tiling_nm_y = vec2(material.tiling_liquid_y_x, material.tiling_liquid_y_y)+tiling;
	//	vec3 normal_texture_x = GetLiquidMap(material, tiling_nm_x);
	//	vec3 normal_texture_y = GetLiquidMap(material, tiling_nm_y);
	//	liquid_normal_from_texture =  mix(normal_texture_x, normal_texture_y, 0.5);
	//	normal_from_texture = GetNormalMap(material, tiling);
	//	vec3 final_normal_map = mix(liquid_normal_from_texture, normal_from_texture, 0.5);
	//	result=normalize(TBN * final_normal_map);
	//}
	return result;

}
vec3 CalculateDirectionalLight(const vec3 normalized_normal, vec4 diffuse_color, vec4 specular_color, vec3 occlusion_color, vec3 emissive_color)
{
	
	vec3 view_pos    = transpose(mat3(matrices.view)) * (-matrices.view[3].xyz);
	vec3 view_dir    = normalize(view_pos - position);
	vec3 light_dir   = normalize(-directional_light.direction );
	vec3 half_dir 	 = normalize(light_dir + view_dir);


	float shadow;	
	
	if(render_shadows)
		shadow = ShadowCalculation();
	else
		shadow = 0;

	float specular = NormalizedSpecular(normalized_normal, half_dir);


		return directional_light.color * (
		(emissive_color
		+ (NormalizedDiffuse(diffuse_color.rgb, specular_color.rgb)
		+ specular_color.rgb * specular ) * shadow)
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

float ShadowCalculation()
{
	//Light frustums
	vec3 normalized_close_depth = close_pos_from_light.xyz / close_pos_from_light.w;
	normalized_close_depth = normalized_close_depth * 0.5 + 0.5;

	vec3 normalized_mid_depth = mid_pos_from_light.xyz / mid_pos_from_light.w;
	normalized_mid_depth = normalized_mid_depth * 0.5 + 0.5;

	vec3 normalized_far_depth = far_pos_from_light.xyz / far_pos_from_light.w;
	normalized_far_depth = normalized_far_depth * 0.5 + 0.5;

	//Perspective camera
	//vec3 normalized_close_cam_pos = pos_from_close_camera.xyz/pos_from_close_camera.w;
	//normalized_close_cam_pos = normalized_close_cam_pos * 0.5 + 0.5;

	//vec3 normalized_mid_cam_pos = pos_from_mid_camera.xyz/pos_from_mid_camera.w;
	//normalized_mid_cam_pos = normalized_mid_cam_pos * 0.5 + 0.5;

	//vec3 normalized_main_cam_pos = pos_from_main_camera.xyz/pos_from_main_camera.w;
	//normalized_main_cam_pos = normalized_main_cam_pos * 0.5 + 0.5;
	
	float bias = 0.005;  
	float factor = 0.0;

	vec3 close_coords = vec3(normalized_close_depth.xy, normalized_close_depth.z - bias);
	vec3 mid_coords = vec3(normalized_mid_depth.xy, normalized_mid_depth.z - bias);
	vec3 far_coords = vec3(normalized_far_depth.xy, normalized_far_depth.z - bias);


	for(int x = -1; x <= 1; ++x) //PCF, solution for edgy shadoWs
    {
        for(int y = -1; y <= 1; ++y)
        {
			//We sample the texture given from the light camera
			//A few times at different texture coordinates

			if(distance_to_camera > 0 && distance_to_camera < far_plane/3)
			{
				close_coords.xy = normalized_close_depth.xy + vec2(x, y)* (1.0 / textureSize(close_depth_map, 0));
				factor += texture(close_depth_map, close_coords);
			}

			if(distance_to_camera >= far_plane/3 &&distance_to_camera < 2*far_plane/3) // Until depth detection is fixed (stops calculating at 50% depth)
			{
				mid_coords.xy = normalized_mid_depth.xy + vec2(x, y)*(1.0 / textureSize(mid_depth_map, 0));
				factor += texture(mid_depth_map, mid_coords);
			}

			if(distance_to_camera >=  2*far_plane/3 && distance_to_camera < far_plane) // Looks weird, but works
			{
				far_coords.xy = normalized_far_depth.xy + vec2(x, y)*(1.0 / textureSize(far_depth_map, 0));
				factor += texture(far_depth_map, far_coords);
			}
			
        }    
    }
    factor /= 9.0;

	return factor;

}

vec3 FrustumsCheck()
{
	vec3 result = vec3(0, 0, 0);
	
	if(distance_to_camera > 0 && distance_to_camera  < far_plane/3)
	{
		result = vec3(100, 0, 0);
	}

	if(distance_to_camera  >= far_plane/3 && distance_to_camera  < 2*far_plane/3) 
	{
		result = vec3(0, 100, 0);
	}
	
	if(distance_to_camera  >= 2*far_plane/3 && distance_to_camera  < far_plane) 
	{
		result = vec3(0, 0, 100);
	}
	return result;
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