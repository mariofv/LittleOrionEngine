#ifndef __QUAD_H__
#define __QUAD_H__

#include <GL/glew.h>
#include <MathGeoLib.h>

class Quad
{
public:
	Quad(float size = 1.f, float depth = 0.f, float2 offset = float2::zero);
	void InitQuadBillboard();
	void InitQuadUI();
	void InitQuadText();

	~Quad();

	void RenderArray() const;
	void RenderElement() const;
	void RenderInstanced(size_t size) const;
private:
	GLuint vao, vbo, ebo;

	float depth = 0.f;
	float half_size = 0.5f;
	float2 offset = float2::zero;
};

#endif //__QUAD_H__