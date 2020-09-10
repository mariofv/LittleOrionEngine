#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

#include <GL/glew.h>

class Viewport
{
public:
	Viewport() = default;
	~Viewport() = default;

public:
	GLuint last_displayed_texture = 0;
};

#endif //_VIEWPORT_H_

