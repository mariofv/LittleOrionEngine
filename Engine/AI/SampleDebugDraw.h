#ifndef _SampleDebugDraw_H_
#define _SampleDebugDraw_H_

#include "AI/DebugDrawGL.h"

class SampleDebugDraw : public DebugDrawGL
{
public:
	unsigned int AreaToCol(unsigned int area);
};

#endif //_SampleDebugDraw_H_