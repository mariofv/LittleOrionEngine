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
void GetCoordinatesInScreenSpace(in vec3 in_coordinates, inout vec4 out_coordinates)
{
    out_coordinates = matrices.proj * vec4(in_coordinates, 1.0) ;
    out_coordinates.xyz /= out_coordinates.w;
    out_coordinates.xy = out_coordinates.xy * 0.5 + 0.5;
}
vec3 BinarySearch(inout vec3 direction, inout vec3 hit_coordinate, inout float depth_diff) {
    
	float depth;
    vec4 projectedCoord;
 
    for(int i = 0; i < MAX_BINARY_SEARCH_COUNT; i++) {
		GetCoordinatesInScreenSpace(hit_coordinate, projectedCoord);
        depth = textureLod(positionMap, projectedCoord.xy,2.0).z;
 
		//search
        depth_diff = hit_coordinate.z - depth;

		direction *= 0.5;
        if(depth_diff > 0.0)
		{
            hit_coordinate += direction;
		}
        else
		{
            hit_coordinate -= direction;  
		}	
    }

    GetCoordinatesInScreenSpace(hit_coordinate, projectedCoord);
 
    return vec3(projectedCoord.xy, depth);
}

vec4 RayCast(in vec3 direction, inout vec3 hit_coordinate, out float depth_diff) {

    direction *= RAY_STEP;

	float depth;
    vec4 projectedCoord;

    //Increase ray lenght until we find something
    for (int i = 0; i < MAX_NUMBER_INCREMENTS; i++) {
        hit_coordinate += direction;
        GetCoordinatesInScreenSpace(hit_coordinate, projectedCoord);

        depth = textureLod(positionMap, projectedCoord.xy,2.0).z;
		if(depth > 1000.0)
		{
			continue;
		}

        depth_diff = hit_coordinate.z - depth;

		//Avoid going over the depth diferent threshold  and check if we have hit something
        if ((direction.z - depth_diff) < 1.2 && depth_diff <= 0.0) 
		{
            return  vec4(projectedCoord.xy, depth,0.0);//vec4(BinarySearch(direction, hit_coordinate, depth_diff),1.0);
		}
    }

    return vec4(projectedCoord.xy, depth,0.0);
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
	vec3 reflection_texture = vec3(0.0);
	if(reflection_strength > 0.0)
	{
		reflection_texture = Reflections(reflection_strength);

	 }
  	  FragColor.rgb = fragment_color.rgb + reflection_texture;
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

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 Reflections(float reflection_strength)
{
#ifndef ENABLE_MSAA
	mat4 invese_view = inverse(matrices.view);

	vec3 view_normal = vec3(texture(normalMap, texCoord));
	vec3 view_position = texture(positionMap, texCoord).xyz;
	vec3 diffuse = texture(screen_texture, texCoord).xyz;

	/*vec3 F0 = vec3(0.04); 
    F0      = mix(F0, diffuse, reflection_strength);
    vec3 Fresnel = FresnelSchlick(max(dot(normalize(view_normal), normalize(view_position)), 0.0), F0);*/

	//Reflection
	vec3 reflected = normalize(reflect(normalize(view_position), normalize(view_normal)));
   
	vec3 hit_position = view_position;
	float depth_diff = 0.0;
	vec4 coords = RayCast(reflected * max(MIN_RAY_STEP, 1.0 -view_position.z), hit_position,depth_diff);

	//Fade edged and ignore results outside screen
	vec2 diff_coords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));
	float screen_edge_factor = clamp(1.0 -(diff_coords.x + diff_coords.y), 0.0, 1.0);
	float Reflection_multiplier = pow(reflection_strength, 3.0) * 
            screen_edge_factor;

	vec3 SSR = texture(screen_texture, coords.xy).xyz * clamp(Reflection_multiplier, 0.0, 0.9);

	return SSR;
 #endif
}
