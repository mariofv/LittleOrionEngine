in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec4 font_color;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(image, TexCoords).r);
	 if(sampled.a < 0.9)
        discard;
    color = font_color * sampled;
}
