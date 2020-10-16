#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include <GL/glew.h>
#include <MathGeoLib/MathGeoLib.h>

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
	void ClearColorAttachement(
		GLenum color_attachement,
		const float3& clear_color = float3::zero,
		GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT
	) const;

	GLint GetDepthAttachement() const;

	void SetMultiSampled(bool multisampled);

	void CheckCompleteness() const;

private:
	void GenerateColorAttachement(float width, float height);
	void GenerateDepthStencilAttachement(float width, float height);
	void LinkAttachements() const;

protected:
	GLuint fbo = 0;
	GLuint color_attachements[5];
	GLuint depth_stencil_attachement;

	int num_color_attachements = 1;
	bool multisampled = false;
};

#endif //_FRAMEBUFFER_H_
