#ifndef _MESH_H_
#define _MESH_H_

#include "Resource.h"

#include <GL/glew.h>
#include <MathGeoLib.h>
#include <vector>

class Metafile;

class Mesh : public Resource
{
public:
	struct Vertex {
		float3 position;
		float2 tex_coords;
		float3 normals;
		float3 tangent;
	};

	Mesh(Metafile* resource_metafile);
	Mesh(Metafile* resource_metafile, std::vector<Vertex> && vertices, std::vector<uint32_t> && indices);
	~Mesh();

	GLuint GetVAO() const;

	int GetNumTriangles() const;
	std::vector<Triangle> GetTriangles() const;

private:
	void LoadInMemory();

public:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

private:
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
};

#endif // !_MESH_H_

