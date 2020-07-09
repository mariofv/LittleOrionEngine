in vec2 v_TexCoords;
in vec4 v_Color;

uniform sampler2D u_glyphTexture;
out vec4 out_FragColor;

void main()
{
    out_FragColor = v_Color;
    out_FragColor.a = texture(u_glyphTexture, v_TexCoords).r;
}
