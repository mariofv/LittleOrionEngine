#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include "MathGeoLib.h"

#include <GL/glew.h>
#include "Resource.h"
#include <ResourceManagement/Loaders/MeshLoader.h>
class Mesh : public Resource
{
public:
	struct Vertex {
		float3 position;
		float2 tex_coords;
		float3 normals;
	};
	Mesh(std::vector<Vertex> && vertices, std::vector<uint32_t> && indices, std::vector<std::string> && meshes_textures_path, std::string mesh_file_path);
	~Mesh();

	GLuint GetVAO() const;
	std::vector<Triangle> GetTriangles() const;


	void Save(Config& config) const override;
	void Load(const Config& config) override;

private:
	void LoadInMemory() override;
public:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<std::string> meshes_textures_path;

private:
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
};

namespace Loader
{
	template<>
	static std::shared_ptr<Mesh> Load(const std::string& uid) {
		return MeshLoader::Load(uid);
	}
}
#endif // !_MESH_H_

