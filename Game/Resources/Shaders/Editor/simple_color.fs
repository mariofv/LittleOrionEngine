uniform vec4 base_color;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 PostProcessFilter;

void main()
{
    FragColor = base_color;
    PostProcessFilter = vec4(0.0, 1.0, 0.0, 0.0);
}
