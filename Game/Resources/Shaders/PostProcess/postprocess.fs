const float gamma = 2.2;
const float W = 11.2;

#if ENABLE_MSAA
uniform sampler2DMS screen_texture;
uniform sampler2DMS post_processing_filter;
#else
uniform sampler2D screen_texture;
uniform sampler2D post_processing_filter;
#endif
uniform sampler2D brightness_texture;

uniform float exposure;

in vec2 texCoord;
layout (location = 0) out vec4 FragColor;

vec3 ToneMapping(vec3 color);
vec3 Uncharted2Tonemap(vec3 x);

void main()
{
#if ENABLE_MSAA
  ivec2 vp = ivec2(vec2(textureSize(screen_texture)) * texCoord);
  vec4 sample1 = texelFetch(screen_texture, vp, 0);
  vec4 sample2 = texelFetch(screen_texture, vp, 1);
  vec4 sample3 = texelFetch(screen_texture, vp, 2);
  vec4 sample4 = texelFetch(screen_texture, vp, 3);

  vec4 fragment_color = (sample1 + sample2 + sample3 + sample4) / 4.0f;

  vp = ivec2(vec2(textureSize(post_processing_filter)) * texCoord);
  sample1 = texelFetch(post_processing_filter, vp, 0);
  sample2 = texelFetch(post_processing_filter, vp, 1);
  sample3 = texelFetch(post_processing_filter, vp, 2);
  sample4 = texelFetch(post_processing_filter, vp, 3);

  vec2 fragment_filter = round((sample1 + sample2 + sample3 + sample4) / 4.0f).xy;

#else
  vec4 fragment_color = texture(screen_texture, texCoord);
  vec2 fragment_filter = texture(post_processing_filter, texCoord).xy;
#endif

#if ENABLE_BLOOM
  vec4 brightness_color = texture(brightness_texture, texCoord);
  fragment_color += brightness_color;
#endif

#if ENABLE_HDR
  fragment_color.rgb = ToneMapping(fragment_color.rgb);
#endif

  FragColor.rgb = fragment_color.rgb;
  FragColor.rgb = pow(FragColor.rgb, vec3(1 / gamma)) * fragment_filter.x + FragColor.rgb *(1- fragment_filter.x);
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
