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
	int num_rendered_lights;
	vec3 light_positions[10];
	vec3 light_colors[10];
} lights;

vec3 compute_light(const int current_light, const Material mat, const vec2 texCoord);

vec4 get_diffuse_color(const Material mat, const vec2 texCoord);
vec3 get_occlusion_color(const Material mat, const vec2 texCoord);
vec3 get_emissive_color(const Material mat, const vec2 texCoord);
vec4 get_specular_color(const Material mat, const vec2 texCoord);

void main()
{
	vec3 final_color = vec3(0.0);

	for(int i = 0; i < lights.num_rendered_lights; i++)
	{
  		final_color += compute_light(i, material, texCoord);
	}

	FragColor = vec4(final_color,1.0);
}

vec3 compute_light(const int current_light, const Material mat, const vec2 texCoord)
{
	vec3 normalized_normal = normalize(normal);

		vec3 light_dir   = normalize(lights.light_positions[current_light] - position);
		float diffuse    = max(0.0, dot(normalized_normal, light_dir));
		float specular   = 0.0;

	if(diffuse > 0.0 && material.k_specular > 0.0 && material.shininess > 0.0)
	{
		vec3 view_pos    = transpose(mat3(matrices.view)) * (-matrices.view[3].xyz);
		vec3 view_dir    = normalize(view_pos - position);
		vec3 half_dir 	 = normalize(light_dir + view_dir);
		float spec       = max(dot(normalized_normal, half_dir), 0.0);

		if(spec > 0.0)
		{
			specular = pow(spec, material.shininess);
		}
	} 

	vec4 diffuse_color  = get_diffuse_color(material, texCoord);
	vec4 specular_color  = get_specular_color(material, texCoord);
	vec3 occlusion_color = get_occlusion_color(material, texCoord);
	vec3 emissive_color  = get_emissive_color(material, texCoord);

	vec3 return_value = lights.light_colors[current_light] * (
		emissive_color
		+ diffuse_color.rgb * (occlusion_color*material.k_ambient)
		+ diffuse_color.rgb * material.k_diffuse * diffuse
		+ specular_color.rgb * material.k_specular * specular
	);

	//return_value = vec3(1.0);

	return return_value;
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
