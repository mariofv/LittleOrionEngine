#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int num_color_attachements) : num_color_attachements(num_color_attachements)
{
	glGenFramebuffers(1, &fbo);
}

FrameBuffer::~FrameBuffer()
{
	ClearAttachements();
	glDeleteFramebuffers(1, &fbo);
}

void FrameBuffer::GenerateAttachements(float width, float height)
{
	GenerateColorAttachement(width, height);
	GenerateDepthAttachement(width, height);
	LinkAttachements();
}

void FrameBuffer::ClearAttachements()
{
	for (size_t i = 0; i < num_color_attachements; ++i)
	{
		if (color_attachements[i] != 0)
		{
			glDeleteTextures(1, &color_attachements[i]);
		}
	}

	if (rbo != 0)
	{
		glDeleteRenderbuffers(1, &rbo);
	}
}

void FrameBuffer::Bind(GLenum target) const
{
	glBindFramebuffer(target, fbo);
}

void FrameBuffer::UnBind(GLenum target)
{
	glBindFramebuffer(target, 0);
}

GLuint FrameBuffer::GetColorAttachement(int index) const
{
	return color_attachements[index];
}

void FrameBuffer::ClearColorAttachement(GLenum color_attachement, const float3& clear_color, GLbitfield mask) const
{
	glDrawBuffer(color_attachement);
	glClearColor(pow(clear_color.x, 2.2f), pow(clear_color.y, 2.2f), pow(clear_color.z, 2.2f), 1.0f);
	glStencilMask(0xFF);
	glClear(mask);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glDrawBuffer(0);
}

void FrameBuffer::GenerateColorAttachement(float width, float height)
{
	glGenTextures(num_color_attachements, &color_attachements[0]);
	for (size_t i = 0; i < num_color_attachements; ++i)
	{
		if (multisampled)
		{
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, color_attachements[i]);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, width, height, GL_TRUE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, color_attachements[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}	
}

void FrameBuffer::GenerateDepthAttachement(float width, float height)
{
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	if (multisampled)
	{
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
	}
	else
	{
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	}
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void FrameBuffer::LinkAttachements() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	for (size_t i = 0; i < num_color_attachements; ++i)
	{
		if (multisampled)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, color_attachements[i], 0);
		}
		else
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, color_attachements[i], 0);
		}
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::SetMultiSampled(bool multisampled)
{
	this->multisampled = multisampled;
}