#ifndef _DEPTHFRAMEBUFFER_H_
#define _DEPTHFRAMEBUFFER_H_

#include "FrameBuffer.h"

class DepthFrameBuffer : public FrameBuffer
{
public:
	void GenerateAttachements(float width, float height) override;
};

#endif //_DEPTHFRAMEBUFFER_H_