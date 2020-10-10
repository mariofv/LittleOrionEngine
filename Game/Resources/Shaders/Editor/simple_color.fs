uniform vec4 base_color;

layout (location = 0) out vec4 FragColor;

void main()
{
    FragColor = base_color;
    FragColor.rgb = pow(FragColor.rgb, vec3(2.2));
}
