#ifndef _SampleDebugDraw_H_
#define _SampleDebugDraw_H_

#include "AI/DebugDrawGL.h"

/// These are just sample areas to use consistent values across the samples.
/// The use should specify these base on his needs.
enum SamplePolyAreas
{
	SAMPLE_POLYAREA_GROUND,
	SAMPLE_POLYAREA_WATER,
	SAMPLE_POLYAREA_ROAD,
	SAMPLE_POLYAREA_DOOR,
	SAMPLE_POLYAREA_GRASS,
	SAMPLE_POLYAREA_JUMP,
};

class SampleDebugDraw : public DebugDrawGL
{
public:
	virtual unsigned int AreaToCol(unsigned int area);
};

#endif //_SampleDebugDraw_H_