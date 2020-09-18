const float gamma = 2.2;

#if ENABLE_MSAA
uniform sampler2DMS screen_texture;
#else
uniform sampler2D screen_texture;
#endif

in vec2 texCoord;
layout (location = 0) out vec4 FragColor;

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

#if ENABLE_HDR
  vec3 mapped = fragment_color.rgb / (fragment_color.rgb + vec3(1.0));
  FragColor = vec4(pow(mapped, vec3(1.0 / gamma)), 1.0);
#else
  FragColor = vec4(pow(fragment_color.rgb, vec3(1.0 / gamma)), 1.0);
#endif
}
