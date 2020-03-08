#ifndef _DEBUGDRAWGL_H_
#define _DEBUGDRAWGL_H_

#include "AI/GLCheckerTexture.h"
#include "recast/DebugUtils/DebugDraw.h"
#include "Component/ComponentCamera.h"

class DebugDrawGL : public DuDebugDraw
{
public:
	virtual void DepthMask(bool state);
	virtual void Texture(bool state);
	virtual void Begin(DuDebugDrawPrimitives prim, float size = 1.0f);
	virtual void Vertex(const float* pos, unsigned int color);
	virtual void Vertex(const float x, const float y, const float z, unsigned int color);
	virtual void Vertex(const float* pos, unsigned int color, const float* uv);
	virtual void Vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v);
	virtual void End();
	virtual void DrawMesh(ComponentCamera& camera);
	virtual void GenerateBuffers();

public:
	GLuint vbo = 0;
	unsigned int vao = 0;

	std::vector<float3> vertices;

	GLCheckerTexture g_tex;
};

#endif //_DEBUGDRAWGL_H_