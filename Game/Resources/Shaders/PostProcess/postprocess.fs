const float gamma = 2.2;
const float W = 11.2;

#if ENABLE_MSAA
uniform sampler2DMS screen_texture;
#else
uniform sampler2D screen_texture;
#endif
uniform sampler2D brightness_texture;

uniform float exposure;

noperspective in vec2 texCoord;
layout (location = 0) out vec4 FragColor;

vec3 ToneMapping(vec3 color);
vec3 Uncharted2Tonemap(vec3 x);
vec3 Reflections(float reflection_strength);

//Reflections
uniform sampler2D normalMap; // in view space
uniform sampler2D positionMap; // in view space
uniform sampler2D ssrValuesMap;

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
	mat4 view;
} matrices;


const int MAX_BINARY_SEARCH_COUNT = 10;
const int MAX_NUMBER_INCREMENTS = 30; // 60
const float RAY_STEP = 0.05; // 0.025
const float MIN_RAY_STEP = 0.2;

// Transform hit_coordinate to screen coordinate
vec4 GetCoordinatesInScreenSpace(vec3 in_coordinates)
{
    vec4 out_coordinates = matrices.proj * inverse(transpose(matrices.view))* vec4(in_coordinates, 1.0);
    out_coordinates.xy /= out_coordinates.w;
    out_coordinates.xy = out_coordinates.xy *0.5 + 0.5;
	return out_coordinates;
}

vec4 RayCast(vec3 direction,vec3 hit_coordinate) {
    //Increase ray lenght until we find something

    for (int i = 0; i < MAX_NUMBER_INCREMENTS; i++) {

        hit_coordinate += direction;
		vec4 projectedCoord = GetCoordinatesInScreenSpace(hit_coordinate);

		float depth = texture(positionMap, projectedCoord.xy).z;
		float depth_diff = (abs(hit_coordinate.z) - abs(depth));
		//Avoid going over the depth diferent threshold  and check if we have hit something
        if ((abs(direction.z) - depth_diff) < 0.0) 
		{		
		  return  vec4(projectedCoord.xy, depth,0.0f);
		}
    }
	return vec4(1.0, 0,0,1.0);

}

void main()
{

#if ENABLE_MSAA
  ivec2 vp = ivec2(vec2(textureSize(screen_texture)) * texCoord);
  vec4 sample1 = texelFetch(screen_texture, vp, 0);
  vec4 sample2 = texelFetch(screen_texture, vp, 1);
  vec4 sample3 = texelFetch(screen_texture, vp, 2);
  vec4 sample4 = texelFetch(screen_texture, vp, 3);

  vec4 fragment_color = (sample1 + sample2 + sample3 + sample4) / 4.0f;
#else
  vec4 fragment_color = texture(screen_texture, texCoord);
#endif

#if ENABLE_BLOOM
  vec4 brightness_color = texture(brightness_texture, texCoord);
  fragment_color += brightness_color;
#endif

#if ENABLE_HDR
  fragment_color.rgb = ToneMapping(fragment_color.rgb);
#endif

	float reflection_strength = texture(ssrValuesMap, texCoord).r;
	vec3 reflection_texture = fragment_color.rgb ;
	if(reflection_strength > 0.0)
	{
		reflection_texture = Reflections(reflection_strength);

	 }
  	  FragColor.rgb =  reflection_texture;
	FragColor.rgb = pow(FragColor.rgb, vec3(1 / gamma));
	FragColor.a = 1.0;

}

vec3 ToneMapping(vec3 color)
{
#if ENABLE_REINHARD
  return color.rgb / (color.rgb + vec3(1.0));

#elif ENABLE_FILMIC
  vec3 hdr = color * exposure;
  vec3 curr = Uncharted2Tonemap(2.0 * hdr);
  vec3 whiteScale = 1.0/Uncharted2Tonemap(vec3(W));
  return curr*whiteScale;

#elif ENABLE_EXPOSURE
  return vec3(1.0) - exp(-color * exposure);

#else
  return color;
#endif
}

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

vec3 Reflections(float reflection_strength)
{
#ifndef ENABLE_MSAA

	vec3 view_normal = vec3(texture(normalMap, texCoord));
	vec3 view_position = vec3(texture(positionMap, texCoord));

	//Reflection
	vec3 reflected = normalize( reflect(normalize(view_position), normalize(view_normal))) ;
   
	vec3 hit_position = view_position;
	vec4 coords = RayCast(reflected , hit_position);

	vec3 SSR = texture(screen_texture, coords.xy).xyz;

	return SSR;
 #endif
}
