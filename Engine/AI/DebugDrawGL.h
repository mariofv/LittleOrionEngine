#ifndef _DEBUGDRAWGL_H_
#define _DEBUGDRAWGL_H_

#include "recast/DebugUtils/DebugDraw.h"
#include "Component/ComponentCamera.h"

class DebugDrawGL
{
public:
	virtual void Vertex(const float* pos, unsigned int color);
	virtual void DrawMesh(ComponentCamera& camera);
	virtual void GenerateBuffers();
	virtual void CleanUp();

public:
	GLuint vbo = 0;
	unsigned int vao = 0;

	std::vector<float3> vertices;

};

#endif //_DEBUGDRAWGL_H_