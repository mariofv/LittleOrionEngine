in vec2 v_TexCoords;
in vec4 v_Color;

uniform sampler2D u_glyphTexture;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 PostProcessFilter;

void main()
{
    FragColor = v_Color;
    FragColor.a = texture(u_glyphTexture, v_TexCoords).r;
    PostProcessFilter = vec4(0.0, 1.0, 0.0, 0.0);
}
