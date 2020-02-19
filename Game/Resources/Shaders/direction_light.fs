#version 430 core

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
	vec3 direction;
	
} light;

vec4 get_diffuse_color(const Material mat, const vec2 texCoord);
vec3 get_occlusion_color(const Material mat, const vec2 texCoord);
vec3 get_emissive_color(const Material mat, const vec2 texCoord);
vec4 get_specular_color(const Material mat, const vec2 texCoord);

void main()
{
	vec3 normalized_normal = normalize(normal);

	vec3 light_dir   = normalize(-light.direction );
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


	vec3 result = light.light_color * (
		emissive_color
		+ diffuse_color.rgb * (occlusion_color*material.k_ambient)
		+ diffuse_color.rgb * material.k_diffuse * diffuse
		+ specular_color.rgb * material.k_specular * specular
	);

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
