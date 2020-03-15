#ifndef _GRID_H_
#define _GRID_H_

#include <MathGeoLib.h>
#include <GL/glew.h>

class Grid
{
public:
	Grid();
	~Grid();

	void Render() const;

private:
	float grid_size = 5000.f;
	float cell_size = 10.f;
	
	float4 thin_lines_color = float4(0.7f, 0.7f, 0.7f, 1.f);
	float4 thick_lines_color = float4::one;

	GLuint grid_vao = 0;
	GLuint grid_vbo = 0;
	GLuint grid_ebo = 0;
};

#endif //_GRID_H_