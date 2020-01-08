#version 330

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv0;
layout(location = 2) in vec3 vertex_normal;

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
	mat4 view;
} matrices;

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

layout (std140) uniform Light
{
	float light_intensity;
	vec3 light_color;
	vec3 light_position;
} light;

flat out float diffuse_intensity;
flat out float specular_intensity;
out vec2 texCoord;

void main()
{
	gl_Position = matrices.proj*matrices.view*matrices.model*vec4(vertex_position, 1.0);
	texCoord = vertex_uv0;

	vec3 position    = (matrices.model*vec4(vertex_position, 1.0)).xyz;
	vec3 normal      = (matrices.model*vec4(vertex_normal, 0.0)).xyz;

	vec3 light_dir   = normalize(light.light_position-position);
	float diffuse    = max(0.0, dot(normal, light_dir));
	float specular   = 0.0;

    if(diffuse > 0.0 && material.k_specular > 0.0 && material.shininess > 0.0)
    {
        vec3 view_pos    = transpose(mat3(matrices.view))*(-matrices.view[3].xyz);
        vec3 view_dir    = normalize(view_pos-position);
        vec3 reflect_dir = reflect(-light_dir, normal);
        float spec       = max(dot(view_dir, reflect_dir), 0.0);

        if(spec > 0.0)
        {
            specular = pow(spec, material.shininess);
        }
    }
    diffuse_intensity = material.k_diffuse*diffuse;
    specular_intensity = material.k_specular*specular;
}
