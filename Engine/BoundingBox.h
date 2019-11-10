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

	void Render(GLuint shader_program) const;

public:
	float3 size;
	float3 center;
	float4x4 model_matrix;
private:
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
};

#endif // _BOUNDINGBOX_H