#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include <GL/glew.h>

class FrameBuffer
{
public:
	FrameBuffer();
	~FrameBuffer();

	void GenerateAttachements(float width, float height);
	void ClearAttachements();

private:
	GLuint fbo = 0;
	GLuint rbo = 0;
	GLuint color_attachement = 0;
};

#endif //_FRAMEBUFFER_H_