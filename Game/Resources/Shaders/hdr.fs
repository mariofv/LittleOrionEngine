
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene_texture;
uniform float exposure;
uniform int hdr_type;const float gamma = 2.2;

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
    vec3 hdrColor = texture(scene_texture, TexCoords).rgb;
	vec3 result;
	switch(hdr_type)
	{
		//REINHARD
		case 0:
			result = hdrColor / (hdrColor + vec3(1.0));
			
		break;
		//FILMIC
		case 1:
			const float W = 11.2;
			hdrColor *= exposure;
			vec3 curr = Uncharted2Tonemap(2.0*hdrColor);
			vec3 whiteScale = 1.0/Uncharted2Tonemap(vec3(W));
			result = curr*whiteScale;
		break;
		//EXPOSURE
		case 2: 
			 result = vec3(1.0) - exp(-hdrColor * exposure);
		break;	
	}

	FragColor = vec4(pow(result, vec3(1.0 / gamma)), 1.0);

}