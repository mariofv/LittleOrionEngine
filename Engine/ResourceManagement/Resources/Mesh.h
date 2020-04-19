#ifndef _MESH_H_
#define _MESH_H_

#include "Resource.h"
#include "ResourceManagement/Manager/MeshManager.h"

#include <GL/glew.h>
#include <MathGeoLib.h>
#include <vector>

class Metafile;

class Mesh : public Resource
{
public:
	struct Vertex 
	{
		float3 position;
		float3 normals;
		float3 tangent;
		float2 tex_coords;
		uint32_t joints[4] = {0,0,0,0};
		float weights[4] = {0,0,0,0};
		uint32_t num_joints = 0;
	};

	Mesh(Metafile* resource_metafile, std::vector<Vertex> && vertices, std::vector<uint32_t> && indices);
	~Mesh();

	GLuint GetVAO() const;

	int GetNumTriangles() const;
	int GetNumVerts() const;
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

namespace ResourceManagement
{
	template<>
	static std::shared_ptr<Mesh> Load(Metafile* metafile, const FileData& resource_data)
	{
		return MeshManager::Load(metafile, resource_data);
	}
}

#endif // !_MESH_H_

