const float gamma = 2.2;
const float W = 11.2;

const float LOG2 = 1.442695;

const int MAX_BINARY_SEARCH_COUNT = 10;
const int MAX_NUMBER_INCREMENTS = 60; // 60
const float MAX_Z_VALUE_DIFFERENCE = 0.002; // 0.025 To make sure that the ray didn't hit to far away'
const float MIN_RAY_STEP = 0.2;

#if ENABLE_MSAA
uniform sampler2DMS screen_texture;
uniform sampler2DMS normal_map; // in view space
uniform sampler2DMS position_map; // in view space
uniform sampler2DMS ssr_value_map;
#else
uniform sampler2D screen_texture;
//Reflections
uniform sampler2D normal_map; // in view space
uniform sampler2D position_map; // in view space
uniform sampler2D ssr_value_map;
#endif
uniform sampler2D brightness_texture;

uniform float exposure;
#if ENABLE_FOG

  #if ENABLE_MSAA
uniform sampler2DMS depth_texture;
  #else
uniform sampler2D depth_texture;
  #endif
#endif

uniform float z_near;
uniform float z_far;
uniform float fog_density;
uniform vec4 fog_color;

in vec2 texCoord;
layout (location = 0) out vec4 FragColor;

#if ENABLE_MSAA
vec4 GetTexture(sampler2DMS texture_uniform,vec2 coordinates);
#else
vec4 GetTexture(sampler2D texture_uniform,vec2 coordinates);
#endif

vec3 ToneMapping(vec3 color);
vec3 Uncharted2Tonemap(vec3 x);
vec3 Reflections(float reflection_strength);

vec4 GetCoordinatesInScreenSpace(vec3 in_coordinates);
float LinearizeDepth(float d);
vec3 BinarySearch(vec3 direction, vec3 hit_coordinate);
vec4 RayCast(vec3 direction,vec3 hit_coordinate);

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
	mat4 view;
} matrices;

void main()
{
  vec4 fragment_color = GetTexture(screen_texture,texCoord);
 #if ENABLE_FOG
  float fragment_depth = GetTexture(depth_texture,texCoord).r;
	fragment_depth =  (2.0 * z_near) / (z_far + z_near - fragment_depth * (z_far - z_near));

  float fog_factor = exp2(- fog_density * fog_density * fragment_depth * fragment_depth * LOG2);
  fog_factor = clamp(fog_factor, 0.0, 1.0);
  fragment_color = mix(fog_color, fragment_color, fog_factor);
#endif

#if ENABLE_BLOOM
  vec4 brightness_color = texture(brightness_texture, texCoord);
  fragment_color += brightness_color;
#endif

#if ENABLE_HDR
  fragment_color.rgb = ToneMapping(fragment_color.rgb);
#endif

	float reflection_strength = GetTexture(ssr_value_map, texCoord).r;

	vec3 reflection_texture = vec3(0.0);

	if(reflection_strength > 0.0)
	{
		reflection_texture = Reflections(reflection_strength);
	}

	FragColor.rgb =  reflection_texture +  fragment_color.rgb;
	FragColor.rgb = pow(FragColor.rgb, vec3(1 / gamma));
	FragColor.a = 1.0;

}

#if ENABLE_MSAA
vec4 GetTexture(sampler2DMS texture_uniform,vec2 coordinates)
#else
vec4 GetTexture(sampler2D texture_uniform,vec2 coordinates)
#endif
{
	#if ENABLE_MSAA
		ivec2 vp = ivec2(vec2(textureSize(texture_uniform)) * coordinates);
		vec4 sample1 = texelFetch(texture_uniform, vp, 0);
		vec4 sample2 = texelFetch(texture_uniform, vp, 1);
		vec4 sample3 = texelFetch(texture_uniform, vp, 2);
		vec4 sample4 = texelFetch(texture_uniform, vp, 3);

		return (sample1 + sample2 + sample3 + sample4) / 4.0f;
	#else
		return texture(texture_uniform, coordinates);
	#endif
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
   const float A = 0.15;
   const float B = 0.50;
   const float C = 0.10;
   const float D = 0.20;
   const float E = 0.02;
   const float F = 0.30;
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

// Transform hit_coordinate to screen coordinate
vec4 GetCoordinatesInScreenSpace(vec3 in_coordinates)
{
    vec4 out_coordinates = matrices.proj* vec4(in_coordinates, 1.0);
    out_coordinates.xy /= out_coordinates.w;
    out_coordinates.xy = out_coordinates.xy *0.5 + 0.5;
	return out_coordinates;
}

//values between [0, z_far]
float LinearizeDepth(float d)
{
	return (matrices.proj[3][2] / (abs(d) - matrices.proj[2][2])); // abs because z is negative in right handed system
}

vec3 BinarySearch(vec3 direction, vec3 hit_coordinate)
{

    vec4 projectedCoord = GetCoordinatesInScreenSpace(hit_coordinate);
    for(int i = 0; i < MAX_BINARY_SEARCH_COUNT; i++)
    {

		float current_ray_depth = LinearizeDepth(hit_coordinate.z);
		float hitted_fragment_depth = LinearizeDepth(GetTexture(position_map,projectedCoord.xy).z);

        direction *= 0.05;
        if(current_ray_depth >= hitted_fragment_depth)
            hit_coordinate -= direction;
        else
            hit_coordinate += direction;

		projectedCoord = GetCoordinatesInScreenSpace(hit_coordinate);
    }

    return vec3(projectedCoord.xy, 1.0);
}

vec4 RayCast(vec3 direction,vec3 hit_coordinate)
{
    //Increase ray lenght until we find something
	float current_hit_distance = 0.0;
	vec4 projectedCoord ;
	float original_depth = LinearizeDepth(hit_coordinate.z);
	float increase_step_limit = MAX_NUMBER_INCREMENTS - MAX_NUMBER_INCREMENTS/4;
	float ray_cast_step = MIN_RAY_STEP;
   for (int i = 0; i < MAX_NUMBER_INCREMENTS; i++) {

		current_hit_distance += ray_cast_step;
        vec3 current_hit_coordinate = hit_coordinate + direction*current_hit_distance;
		projectedCoord = GetCoordinatesInScreenSpace(current_hit_coordinate);

		float current_ray_depth = LinearizeDepth(current_hit_coordinate.z);
		float hitted_fragment_depth = LinearizeDepth(GetTexture(position_map,projectedCoord.xy).z);
		float depth_diff = (current_ray_depth - hitted_fragment_depth);

		//Avoid going over the depth diferent threshold  and check if we have hit something
        if (current_ray_depth >= hitted_fragment_depth && depth_diff < MAX_Z_VALUE_DIFFERENCE && original_depth < hitted_fragment_depth)
		{
			return vec4(BinarySearch(direction,current_hit_coordinate), 1.0);
		}else if(i == increase_step_limit) // If we are almost finish and there is not hit lets make the ray bigger, maybe things are too far away
		{
			ray_cast_step *=4;  
		}
    }

	return vec4(texCoord,0,0.0);
}

vec3 Reflections(float reflection_strength)
{
	vec3 view_normal = vec3(GetTexture(normal_map, texCoord) * inverse(matrices.view));
	vec3 view_position = vec3(GetTexture(position_map, texCoord));

	//Reflection
	vec3 reflected = normalize( reflect(normalize(view_position), normalize(view_normal))) ;

	vec3 hit_position = view_position;
	vec4 coords = RayCast(reflected , hit_position);

	vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));
	float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0) * reflection_strength;


	vec3 SSR = GetTexture(screen_texture,coords.xy).xyz * screenEdgefactor;
	return SSR;
}
