#ifndef _MESH_H_
#define _MESH_H_

#include "Resource.h"
#include "ResourceManagement/Manager/MeshManager.h"

#include <GL/glew.h>
#include <MathGeoLib.h>
#include <vector>

class Metafile;

static const size_t MAX_JOINTS = 4;

enum UVChannel
{
	TEXTURE,
	LIGHTMAP,
	TOTALUVS
};
class Mesh : public Resource
{
public:
	struct Vertex 
	{
		float3 position;
		float3 normals;
		float3 tangent;
		float3 bitangent;
		float2 tex_coords[UVChannel::TOTALUVS];
		uint32_t joints[MAX_JOINTS] = {0,0,0,0};
		float weights[MAX_JOINTS] = {0,0,0,0};
		uint32_t num_joints = 0;
	};

	Mesh(uint32_t uuid, std::vector<Vertex> && vertices, std::vector<uint32_t> && indices, bool async = false);
	~Mesh();

	GLuint GetVAO() const;
	GLuint GetEBO() const;
	int GetNumTriangles() const;
	int GetNumVerts() const;
	std::vector<Triangle> GetTriangles() const;

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
	static std::shared_ptr<Mesh> Load(uint32_t uuid, const FileData& resource_data, bool async)
	{
		return MeshManager::Load(uuid, resource_data, async);
	}
}

#endif // !_MESH_H_

