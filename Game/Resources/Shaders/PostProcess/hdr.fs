

uniform bool bloom;

struct HRD_EFFECT
{
  sampler2D scene_texture;
  sampler2D bloom_texture;
};

uniform HRD_EFFECT hdr_uniform;



void main()
{
  vec3 hdr_color = texture(hdr_uniform.scene_texture, TexCoords).rgb;
	vec3 bloom_color = texture(hdr_uniform.bloom_texture, TexCoords).rgb;
	vec3 result;
	if(bloom)
	{
		hdr_color += bloom_color; // additive blending
	}
}
