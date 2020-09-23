in  vec4 v_Color;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 PostProcessFilter;

void main()
{
    FragColor = v_Color;
    PostProcessFilter = vec4(0.0, 1.0, 0.0, 0.0);
}
