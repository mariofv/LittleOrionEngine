#include "FrameBuffer.h"
#include "Log/EngineLog.h"

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
	GenerateDepthStencilAttachement(width, height);
	LinkAttachements();
	CheckCompleteness();
}

void FrameBuffer::ClearAttachements()
{
	glDeleteTextures(num_color_attachements, &color_attachements[0]);

	if (depth_stencil_attachement != 0)
	{
		glDeleteTextures(1, &depth_stencil_attachement);
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

GLint FrameBuffer::GetDepthAttachement() const
{
	return depth_stencil_attachement;
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

void FrameBuffer::GenerateDepthStencilAttachement(float width, float height)
{
	glGenTextures(1, &depth_stencil_attachement);
	if (multisampled)
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depth_stencil_attachement);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH24_STENCIL8, width, height, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, depth_stencil_attachement);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
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

	if (multisampled)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depth_stencil_attachement, 0);
	}
	else
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_stencil_attachement, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::SetMultiSampled(bool multisampled)
{
	this->multisampled = multisampled;
}

void FrameBuffer::CheckCompleteness() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (fbo_status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		APP_LOG_INFO("Framebuffer is complete");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		APP_LOG_ERROR("Framebuffer attachements are incomplete");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		APP_LOG_ERROR("Framebuffer has no images attached");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		APP_LOG_ERROR("Framebuffer has imcomplete draw buffers");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		APP_LOG_ERROR("Framebuffer has imcomplete read buffers");
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		APP_LOG_ERROR("Framebuffer has unsupported internal format");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		APP_LOG_ERROR("Framebuffer textures don't share sampling ratio");
		break;

	default:
		APP_LOG_ERROR("What did you do?");
		break;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
