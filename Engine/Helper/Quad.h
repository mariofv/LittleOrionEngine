#pragma once


#include <GL/glew.h>
class Quad
{
public:
	Quad() = default;
	void InitQuadBillboard();
	void InitQuadUI();
	~Quad();

	void RenderArray() const;
	void RenderElement() const;
	void RenderInstanced(size_t size) const;
private:
	GLuint vao, vbo, ebo;
};

