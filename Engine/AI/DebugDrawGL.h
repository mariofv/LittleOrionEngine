#ifndef _DEBUGDRAWGL_H_
#define _DEBUGDRAWGL_H_

#include "AI/GLCheckerTexture.h"
#include "recast/DebugUtils/DebugDraw.h"
#include "Component/ComponentCamera.h"

class DebugDrawGL : public duDebugDraw
{
public:
	virtual void depthMask(bool state);
	virtual void texture(bool state);
	virtual void begin(duDebugDrawPrimitives prim, float size = 1.0f);
	virtual void vertex(const float* pos, unsigned int color);
	virtual void vertex(const float x, const float y, const float z, unsigned int color);
	virtual void vertex(const float* pos, unsigned int color, const float* uv);
	virtual void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v);
	virtual void end();
	virtual void drawMesh(ComponentCamera& camera);

public:
	unsigned int vbo = 0;
	unsigned int vao = 0;

	std::vector<float3> vertices;

	GLCheckerTexture g_tex;
};

#endif //_DEBUGDRAWGL_H_