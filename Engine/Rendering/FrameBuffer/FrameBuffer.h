#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include <GL/glew.h>

class FrameBuffer
{
public:
	FrameBuffer(int num_color_attachements = 1);
	virtual ~FrameBuffer();

	virtual void GenerateAttachements(float width, float height);
	void ClearAttachements();

	void Bind(GLenum target = GL_FRAMEBUFFER) const;
	static void UnBind(GLenum target = GL_FRAMEBUFFER);

	GLuint GetColorAttachement(int index = 0) const;

	void SetMultiSampled(bool multisampled);
	void SetFloatingPoint(bool floating_point);

private:
	void GenerateColorAttachement(float width, float height);
	void GenerateDepthAttachement(float width, float height);
	void LinkAttachements() const;

protected:
	GLuint fbo = 0;
	GLuint rbo = 0;
	GLuint color_attachements[2];

	int num_color_attachements = 1;
	bool multisampled = false;
	bool floating_point = false;
};

#endif //_FRAMEBUFFER_H_