in vec3 in_Position;
in vec4 in_ColorPointSize;

out vec4 v_Color;
uniform mat4 u_MvpMatrix;

void main()
{
    gl_Position  = u_MvpMatrix * vec4(in_Position, 1.0);
    gl_PointSize = in_ColorPointSize.w;
    v_Color      = vec4(in_ColorPointSize.xyz, 1.0);
}
