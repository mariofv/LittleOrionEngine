out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene_image;
uniform sampler2D blur_image;
uniform bool bloom;

void main()
{             
    vec3 hdrColor = texture(scene, TexCoords).rgb;      
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    hdrColor += bloomColor; // additive blending
    FragColor = vec4(hdrColor, 1.0);
}