#ifndef _BOUNDINGBOX_H
#define _BOUNDINGBOX_H

#include "MathGeoLib.h"
#include <GL/glew.h>

class BoundingBox
{
public:
	BoundingBox() = default;
	BoundingBox(const float3 min_coords, const float3 max_coords);

	~BoundingBox();

	void Render(const GLuint shader_program) const;

public:
	float3 size = float3::zero;
	float3 center = float3::zero;

private:
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
};

#endif // _BOUNDINGBOX_H