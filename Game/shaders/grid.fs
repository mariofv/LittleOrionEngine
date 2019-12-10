#version 330 core

#define PI 3.141592
// size of a square in pixel
#define N 20.0

in vec2 fragCoord;
out vec4 fragColor;

void main()
{    
    // the grid in itself
    vec2 Coord = cos(PI/N*fragCoord);
	fragColor = vec4(1.0)-0.5*smoothstep(0.9,1.0,max(Coord.x,Coord.y));
}