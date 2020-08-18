
layout (location = 0) out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D scene_texture;
uniform sampler2D bloom_texture;
uniform float exposure;
uniform int hdr_type;uniform bool bloom;const float gamma = 2.2;

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
	
	FragColor = vec4(pow(hdrColor, vec3(1.0 / gamma)), 1.0);
	

}