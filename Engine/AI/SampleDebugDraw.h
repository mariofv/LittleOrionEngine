#ifndef _SampleDebugDraw_H_
#define _SampleDebugDraw_H_

#include "AI/DebugDrawGL.h"

class SampleDebugDraw : public DebugDrawGL
{
public:
	virtual unsigned int areaToCol(unsigned int area);
};

#endif //_SampleDebugDraw_H_