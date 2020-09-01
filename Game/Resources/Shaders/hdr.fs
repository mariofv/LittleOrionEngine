
layout (location = 0) out vec4 FragColor;
in vec2 TexCoords;


uniform float exposure;
uniform int hdr_type;uniform bool bloom;uniform bool hdr_active;const float gamma = 2.2;

struct HRD_EFFECT
{
sampler2D scene_texture;
sampler2D bloom_texture;
};
uniform HRD_EFFECT hdr_uniform;
vec3 Uncharted2Tonemap(vec3 x)
{
 const float A = 0.22;
 const float B = 0.3;
 const float C = 0.1;
 const float D = 0.2;
 const float E = 0.01;
 const float F = 0.30;
 return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main()
{    
    vec3 hdr_color = texture(hdr_uniform.scene_texture, TexCoords).rgb;
	vec3 bloom_color = texture(hdr_uniform.bloom_texture, TexCoords).rgb;
	vec3 result;
	if(bloom)
	{
		hdr_color += bloom_color; // additive blending
	}
	if(hdr_active)
	{
		switch(hdr_type)
		{
			//REINHARD
			case 0:
				result = hdr_color / (hdr_color + vec3(1.0));
			
			break;
			//FILMIC
			case 1:
				const float W = 11.2;
				hdr_color *= exposure;
				vec3 curr = Uncharted2Tonemap(2.0*hdr_color);
				vec3 whiteScale = 1.0/Uncharted2Tonemap(vec3(W));
				result = curr*whiteScale;
			break;
			//EXPOSURE
			case 2: 
				 result = vec3(1.0) - exp(-hdr_color * exposure);
			break;	
		}
	}


	FragColor = vec4(pow(result, vec3(1.0 / gamma)), 1.0);

}