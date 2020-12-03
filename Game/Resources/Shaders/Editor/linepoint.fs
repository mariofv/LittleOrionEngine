in  vec4 v_Color;
layout (location = 0) out vec4 FragColor;

void main()
{
    FragColor = v_Color;
    FragColor.rgb = pow(FragColor.rgb, vec3(2.2));
}
