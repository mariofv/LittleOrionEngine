#ifndef _DEBUGDRAWGL_H_
#define _DEBUGDRAWGL_H_

#include "Component/ComponentCamera.h"
#include <recast/DebugUtils/DebugDraw.h>

class DebugDrawGL
{
	struct NavMeshVertex
	{
		float3 vertex;
		float4 color;
	};
public:
	virtual void Vertex(const float* pos, unsigned int color);
	virtual float4 GetColorRGBA(unsigned int color) const;
	virtual void DrawMesh(ComponentCamera& camera);
	virtual void GenerateBuffers();
	virtual void CleanUp();

public:
	GLuint vbo = 0;
	GLuint vbo_color = 0;
	unsigned int vao = 0;
	std::vector<float4> colors;

	std::vector<float3> vertices;

};

#endif //_DEBUGDRAWGL_H_