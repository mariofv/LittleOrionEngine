in vec2 v_TexCoords;
in vec4 v_Color;

uniform sampler2D u_glyphTexture;
layout (location = 0) out vec4 FragColor;

void main()
{
    FragColor = v_Color;
    FragColor.a = texture(u_glyphTexture, v_TexCoords).r;
    FragColor.rgb = pow(FragColor.rgb, vec3(2.2));
}
