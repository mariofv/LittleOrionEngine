#version 430 core

in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec3 tangent;

subroutine vec3 normal_subroutine();
layout (location=0) subroutine uniform normal_subroutine NormalSoubroutine;

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
	float alpha_blending;
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
vec3 GetNormalMap(sampler2D normal_map, const vec2 texCoord);

//TYPE OF LIGHTS
vec3 CalculateDirectionalLight(const vec3 normalized_normal);
vec3 CalculateSpotLight(SpotLight spot_light, const vec3 normalized_normal);
vec3 CalculatePointLight(PointLight point_light, const vec3 normalized_normal);

//BRDF
float DistributionGGX(vec3 normal, vec3 half_dir, float roughness);
float GeometrySchlick(float Ndot, float roughness);
float GeometrySmith(vec3 normal, vec3 view_pos, vec3 light_dir, float roughness);
vec3 Fresnel(vec3 light_dir, vec3 normal, float metalness);
vec3 BRDF(vec3 view_pos, vec3 normal, vec3 half_dir, vec3 light_dir, float roughness, float metalness);

mat3 CreateTangentSpace(const vec3 normal, const vec3 tangent);

//SUBROUTINES
layout(index=0) subroutine (normal_subroutine) vec3 ComputeMaterialWithNormalMap()
{
	vec3 normalized_normal = normalize(GetNormalMap(material.normal_map, texCoord));
	return normalized_normal = CreateTangentSpace(normalize(normal), normalize(tangent)) * normalized_normal;
}

layout(index=1) subroutine (normal_subroutine) vec3 ComputeMaterialWithoutNormalMap()
{
	return normalize(normal);
}

void main()
{


	//vec3 normalized_normal = normalize(GetNormalMap(material.normal_map, texCoord));
	//normalized_normal = CreateTangentSpace(normalize(normal), normalize(tangent)) * normalized_normal;
	vec3 normalized_normal = normalize(normal);
	vec3 result = vec3(0);

	for (int i = 0; i < directional_light.num_directional_lights; ++i)
	{
		result += CalculateDirectionalLight(normal);
	}

	for (int i = 0; i < num_spot_lights; ++i)
	{
		result += CalculateSpotLight(spot_lights[i], normal);
	}

	for (int i = 0; i < num_point_lights; ++i)
	{
		result += CalculatePointLight(point_lights[i], normal);	
	}

	//FragColor = vec4(vec3(normalize(tangent)),1.0);
	FragColor = vec4(result,1.0);
	FragColor.a=material.alpha_blending;

	
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

vec3 GetNormalMap(sampler2D normal_map, const vec2 texCoord)
{
	return texture(normal_map, texCoord).rgb*2.0-1.0;
}

vec3 CalculateDirectionalLight(const vec3 normalized_normal)
{

	vec3 light_dir   = normalize(-directional_light.direction );
	float diffuse    = max(0.0, dot(normalized_normal, light_dir));
	float specular   = 0.0;
	vec3 view_pos    = transpose(mat3(matrices.view)) * (-matrices.view[3].xyz);
	vec3 view_dir    = normalize(view_pos - position);
	vec3 half_dir 	 = normalize(light_dir + view_dir);

	if(diffuse > 0.0 && material.k_specular > 0.0 && material.specular_color.w > 0.0)
	{
		
		float spec       = max(dot(normalized_normal, half_dir), 0.0);

		if(spec > 0.0)
		{
			specular = pow(spec, material.specular_color.w);
		}
	}

	vec4 diffuse_color  = GetDiffuseColor(material, texCoord);
	vec4 specular_color  = GetSpecularColor(material, texCoord);
	vec3 occlusion_color = GetOcclusionColor(material, texCoord);
	vec3 emissive_color  = GetEmissiveColor(material, texCoord);

	return directional_light.color * (
		emissive_color
		+ diffuse_color.rgb * (occlusion_color*material.k_ambient)
		+ diffuse_color.rgb * material.k_diffuse * diffuse
		+ specular_color.rgb * material.k_specular * BRDF(view_dir, normalized_normal, half_dir, light_dir, material.roughness, material.metalness)
	);
}

vec3 CalculateSpotLight(SpotLight spot_light, const vec3 normalized_normal)
{
	vec3 light_dir   = normalize(spot_light.position - position);
    float diffuse    = max(0.0, dot(normalized_normal, light_dir));
    float specular   = 0.0;
	vec3 view_pos    = transpose(mat3(matrices.view)) * (-matrices.view[3].xyz);
	vec3 view_dir    = normalize(view_pos - position);
	vec3 half_dir      = normalize(light_dir + view_dir);

	if(diffuse > 0.0 && material.k_specular > 0.0 && material.specular_color.w > 0.0)
	{
		
		float spec       = max(dot(normalized_normal, half_dir), 0.0);

		if(spec > 0.0)
		{
			specular = pow(spec, material.specular_color.w);
		}
	} 

    vec4 diffuse_color  = GetDiffuseColor(material, texCoord);
	vec4 specular_color  = GetSpecularColor(material, texCoord);
	vec3 occlusion_color = GetOcclusionColor(material, texCoord);
	vec3 emissive_color  = GetEmissiveColor(material, texCoord);

    float theta = dot(light_dir, normalize(-spot_light.direction)); 
    float epsilon = (spot_light.cutOff - spot_light.outerCutOff);
    float intensity = clamp((theta - spot_light.outerCutOff) / epsilon, 0.0, 1.0);
    
    float distance    = length(spot_light.position - position);
	//TODO: Why is this commented?
    //float attenuation = 1.0 / (spot_light.constant + spot_light.linear * distance + 
                //spot_light.quadratic * (distance * distance));    

   return spot_light.color * (
        emissive_color
        + diffuse_color.rgb * (occlusion_color*material.k_ambient)//*attenuation
        + diffuse_color.rgb * material.k_diffuse * diffuse*intensity//*attenuation
        + specular_color.rgb * material.k_specular * BRDF(view_dir, normalized_normal, half_dir, light_dir, material.roughness, material.metalness)*intensity//*attenuation
    );

}

vec3 CalculatePointLight(PointLight point_light, const vec3 normalized_normal)
{
	vec3 light_dir   = normalize(point_light.position - position);
	float diffuse    = max(0.0, dot(normalized_normal, light_dir));
	float specular   = 0.0;

    vec3 view_pos    = transpose(mat3(matrices.view)) * (-matrices.view[3].xyz);
    vec3 view_dir    = normalize(view_pos - position);
	vec3 half_dir 	 = normalize(light_dir + view_dir);
    float spec       = max(dot(normalized_normal, half_dir), 0.0);
	specular		 = pow(spec, material.specular_color.w);


	vec4 diffuse_color  = GetDiffuseColor(material, texCoord);
	vec4 specular_color  = GetSpecularColor(material, texCoord);
	vec3 occlusion_color = GetOcclusionColor(material, texCoord);
	vec3 emissive_color  = GetEmissiveColor(material, texCoord);


	float distance    = length(point_light.position - position);
	float attenuation = 1.0 / (point_light.constant + point_light.linear * distance + 
    		    point_light.quadratic * (distance));    

	return point_light.color * (
		emissive_color
		+  diffuse_color.rgb * (occlusion_color*material.k_ambient)*attenuation
		+ diffuse_color.rgb * material.k_diffuse * diffuse * attenuation
		+ specular_color.rgb * material.k_specular * BRDF(view_dir, normalized_normal, half_dir, light_dir, material.roughness, material.metalness)* attenuation
	);

}

mat3 CreateTangentSpace(const vec3 normal, const vec3 tangent)
{
	 vec3 ortho_tangent = normalize(tangent-dot(tangent, normal)*normal); // Gram-Schmidt
	 vec3 bitangent = cross(normal, ortho_tangent);
	return mat3(tangent, bitangent, normal);
}

//BRDF

//Distribution Function GGX
float DistributionGGX(vec3 normal, vec3 half_dir, float roughness)
{
    float r     = roughness*roughness;
	float r2 = r*r;

    float NdotH  = max(dot(normal, half_dir), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = r2;
    float denom  = (NdotH2 * (r2 - 1.0) + 1.0);
    denom        = 3.14159 * denom * denom;
	
    return nom / denom;
}

//Geometry Functions
float GeometrySchlick(float Ndot, float roughness)
{
	float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = Ndot;
    float denom = Ndot * (1.0 - k) + k;
	
    return nom / denom;
}

float GeometrySmith(vec3 normal, vec3 view_dir, vec3 light_dir, float roughness)
{
   
   float NdotV = max(dot(normal, view_dir), 0.0);
    float NdotL = max(dot(normal, light_dir), 0.0);

    float ggx1 = GeometrySchlick(NdotV, roughness);
    float ggx2 = GeometrySchlick(NdotL, roughness);
	
    return ggx1 * ggx2;
}

//Schlick
vec3 Fresnel(vec3 light_dir, vec3 normal, float metalness)
{

	vec3 F0 = vec3(metalness); 
	float cosTheta = max(dot(light_dir, normal), 0.0);

    return F0 + (1.0 - F0) * pow((1.0 - cosTheta), 5.0);
}

//Cook-Torrance
vec3 BRDF(vec3 view_dir, vec3 normal, vec3 half_dir, vec3 light_dir, float roughness, float metalness)
{
	
	float D = DistributionGGX(normal, half_dir, roughness);
	float G = GeometrySmith(normal, view_dir, light_dir, roughness);
	vec3 F = Fresnel(light_dir, normal, metalness);

	vec3 num = D*G*F;

	float denom = 4.0 * max(dot(normal, view_dir), 0.0) * max(dot(normal, light_dir), 0.0);
	vec3 specular = num / max(denom, 0.001); 
	
	return specular;	
}







