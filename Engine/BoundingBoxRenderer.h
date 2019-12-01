#ifndef _BOUNDINGBOXRENDERER_H
#define _BOUNDINGBOXRENDERER_H

#include "MathGeoLib.h"
#include <GL/glew.h>

class ComponentCamera;

class BoundingBoxRenderer
{
public:
	BoundingBoxRenderer();
	~BoundingBoxRenderer();

	void Render(const ComponentCamera &camera, const AABB bounding_box, const GLuint shader_program) const;

private:
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
};

#endif // _BOUNDINGBOXRENDERER_H