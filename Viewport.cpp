#include "Viewport.h"

#include "FrameBuffer.h"

Viewport::Viewport()
{
	render_fbo = new FrameBuffer();
}

Viewport::~Viewport()
{
	delete render_fbo;
}

void Viewport::SetSize(float width, float height)
{
	if (this->width == width && this->height == height)
	{
		return;
	}

	this->width = width;
	this->height = height;

	render_fbo->ClearAttachements();
	render_fbo->GenerateAttachements(width, height);
}