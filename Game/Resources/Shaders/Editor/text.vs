in vec2 in_Position;
in vec2 in_TexCoords;
in vec3 in_Color;

uniform vec2 u_screenDimensions;

out vec2 v_TexCoords;
out vec4 v_Color;

void main()
{
    // Map to normalized clip coordinates:
    float x = ((2.0 * (in_Position.x - 0.5)) / u_screenDimensions.x) - 1.0;
    float y = 1.0 - ((2.0 * (in_Position.y - 0.5)) / u_screenDimensions.y);

    gl_Position = vec4(x, y, 0.0, 1.0);
    v_TexCoords = in_TexCoords;
    v_Color     = vec4(in_Color, 1.0);
}
