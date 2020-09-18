#pragma once


#include <GL/glew.h>
class Quad
{
public:
	Quad(float size = 1.f, float depth = 0.f);
	~Quad();

	void Render() const;
private:
	GLuint vao, vbo, ebo;
};

