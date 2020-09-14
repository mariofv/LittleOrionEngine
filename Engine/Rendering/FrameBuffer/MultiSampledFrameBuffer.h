#ifndef _MULTISAMPLEDFRAMEBUFFER_H_
#define _MULTISAMPLEDFRAMEBUFFER_H_

#include "FrameBuffer.h"

class MultiSampledFrameBuffer : public FrameBuffer
{
public:
	void GenerateAttachements(float width, float height) override;
};

#endif //_MULTISAMPLEDFRAMEBUFFER_H_