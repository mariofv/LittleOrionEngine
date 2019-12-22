#ifndef _GRIDRENDERER_H_
#define _GRIDRENDERER_H_

#include "Component/ComponentCamera.h"
#include "Texture.h"

#include "MathGeoLib.h"
#include <GL/glew.h>

class ComponentCamera;

class GridRenderer
{
public:
	GridRenderer();
	~GridRenderer();
	
	void InitGridPlane();
	void InitGridTexture();
	void Render(const ComponentCamera &camera) const;

public:
	float size = 2048.f;

private:
	GLuint vbo = 0;
	GLuint vao = 0;
	GLuint ebo = 0;
	std::shared_ptr<Texture> texture = nullptr;

};

#endif // _GRIDRENDERER_H_