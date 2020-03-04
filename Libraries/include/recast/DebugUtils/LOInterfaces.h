#ifndef LOINTERFACES_H
#define LOINTERFACES_H

#include "Recast.h"
#include "RecastDump.h"
#include "MathGeoLib/MathGeoLib.h"

class LOInterfaces
{
public:
	LOInterfaces();
	~LOInterfaces();
	/*virtual unsigned int areaToCol(unsigned int area);
	virtual void depthMask(bool state);
	virtual void texture(bool state);
	virtual void begin(duDebugDrawPrimitives prim, float size = 1.0f);
	virtual void vertex(const float* pos, unsigned int color);
	virtual void vertex(const float x, const float y, const float z, unsigned int color);
	virtual void vertex(const float* pos, unsigned int color, const float* uv);
	virtual void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v);
	virtual void end();*/

private:
	unsigned vbo = 0;
	unsigned vao = 0;
	unsigned shader = 0;
	std::vector<float3> vertices;
};

#endif //LOINTERFACES_H