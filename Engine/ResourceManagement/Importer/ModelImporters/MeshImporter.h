#ifndef _MESHIMPORTER_H_
#define _MESHIMPORTER_H_

#include "ResourceManagement/Importer/Importer.h"
#include "ResourceManagement/Resources/Mesh.h"
#include <vector>
#include <string>

#include <assimp/mesh.h>
class Skeleton;
class MeshImporter : public Importer
{
public:
	MeshImporter() = default;
	~MeshImporter() = default;
	ImportResult Import(const File & file, bool force = false) const;
	ImportResult ImportMesh(const aiMesh* assimp_mesh, const aiMatrix4x4& mesh_transformation, const std::string& imported_file, float unit_scale_factor, const Skeleton & skeleton) const;

private:
	std::vector<std::pair<std::vector<uint32_t>, std::vector<float>>> GetSkinning(const aiMesh* assimp_mesh, const Skeleton & skeleton) const;
	void SaveBinary(std::vector<Mesh::Vertex> && vertices, std::vector<uint32_t> && indices, const std::string& exported_file, const std::string& imported_file) const;
};
#endif // !_MESHIMPORTER_H_