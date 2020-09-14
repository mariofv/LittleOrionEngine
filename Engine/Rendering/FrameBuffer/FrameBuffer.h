#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include <GL/glew.h>

class FrameBuffer
{
public:
	FrameBuffer();
	virtual ~FrameBuffer();

	virtual void GenerateAttachements(float width, float height);
	void ClearAttachements();

	void Bind(GLenum target = GL_FRAMEBUFFER) const;
	static void UnBind(GLenum target = GL_FRAMEBUFFER);

	GLuint GetColorAttachement() const;

protected:
	GLuint fbo = 0;
	GLuint rbo = 0;
	GLuint color_attachement = 0;
};

#endif //_FRAMEBUFFER_H_