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

layout (std140) uniform Light
{
	vec3 light_color;
	vec3 light_position;
	 float constant;
    float linear;
    float quadratic;
} light;


vec4 get_diffuse_color(const Material mat, const vec2 texCoord);
vec3 get_occlusion_color(const Material mat, const vec2 texCoord);
vec3 get_emissive_color(const Material mat, const vec2 texCoord);
vec4 get_specular_color(const Material mat, const vec2 texCoord);
const float screenGamma = 2.2; // Assume the monitor is calibrated to the sRGB color space

void main()
{
	vec3 normalized_normal = normalize(normal);

	vec3 light_dir   = normalize(light.light_position - position);
	float diffuse    = max(0.0, dot(normalized_normal, light_dir));
	float specular   = 0.0;

    vec3 view_pos    = transpose(mat3(matrices.view)) * (-matrices.view[3].xyz);
    vec3 view_dir    = normalize(view_pos - position);
	vec3 half_dir 	 = normalize(light_dir + view_dir);
    float spec       = max(dot(normalized_normal, half_dir), 0.0);
	specular		 = pow(spec, material.shininess);


	vec4 diffuse_color  = get_diffuse_color(material, texCoord);
	vec4 specular_color  = get_specular_color(material, texCoord);
	vec3 occlusion_color = get_occlusion_color(material, texCoord);
	vec3 emissive_color  = get_emissive_color(material, texCoord);


	float distance    = length(light.light_position - position);
	float attenuation = 1.0 / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance));    


	vec3 result = light.light_color * (
		emissive_color
		+  diffuse_color.rgb * (occlusion_color*material.k_ambient)*attenuation
		+ diffuse_color.rgb * material.k_diffuse * diffuse * attenuation
		+ specular_color.rgb * material.k_specular * specular* attenuation
	);

	vec3 colorGammaCorrected = pow(result, vec3(1.0 / screenGamma));
	FragColor = vec4(result,1.0);
}

vec4 get_diffuse_color(const Material mat, const vec2 texCoord)
{
	return texture(mat.diffuse_map, texCoord)*mat.diffuse_color;
}

vec4 get_specular_color(const Material mat, const vec2 texCoord)
{
	return texture(mat.specular_map, texCoord)*mat.specular_color;
}

vec3 get_occlusion_color(const Material mat, const vec2 texCoord)
{
	return texture(mat.occlusion_map, texCoord).rgb * vec3(1.0,1.0,1.0);
}

vec3 get_emissive_color(const Material mat, const vec2 texCoord)
{
	return (texture(mat.emissive_map, texCoord)*mat.emissive_color).rgb;
}
