out vec4 color;

void main()
{
    color = vec4(0.0, 0.0, 0.0, 1.0);
#if NORMAL_MAP
    color += vec4(1.0, 0.0, 0.0, 0.0);
#endif

#if SPECULAR_MAP
    color += vec4(0.0, 1.0, 0.0, 0.0);
#endif

#if RECEIVE_SHADOWS
    color += vec4(0.0, 0.0, 1.0, 0.0);
#endif
}
