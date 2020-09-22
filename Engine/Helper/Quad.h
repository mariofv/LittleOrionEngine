#pragma once


#include <GL/glew.h>
class Quad
{
public:
	Quad(float size = 1.f, float depth = 0.f);
	void InitQuadBillboard();
	void InitQuadUI();

	~Quad();

	void RenderArray() const;
	void RenderElement() const;
	void RenderInstanced(size_t size) const;
private:
	GLuint vao, vbo, ebo;

	float depth = 0.f;
	float half_size = 0.5f;
};
