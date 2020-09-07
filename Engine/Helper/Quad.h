#pragma once


#include <GL/glew.h>
class Quad
{
public:
	Quad();
	~Quad();

	void Render() const;
private:
	GLuint vao, vbo;
};

