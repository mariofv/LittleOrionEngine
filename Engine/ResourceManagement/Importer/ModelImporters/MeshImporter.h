#ifndef _MESHIMPORTER_H_
#define _MESHIMPORTER_H_

#include "ResourceManagement/Importer/Importer.h"
#include "ResourceManagement/Resources/Mesh.h"

#include <vector>
#include <string>

#include <assimp/mesh.h>

class MeshImporter : public Importer
{
public:
	MeshImporter() : Importer(ResourceType::MESH) {};
	~MeshImporter() = default;

	FileData ExtractData(Path& assets_file_path) const override;
	FileData ExtractMeshFromAssimp(const aiMesh* assimp_mesh, const aiMatrix4x4& mesh_transformation, float unit_scale_factor, uint32_t mesh_skeleton_uuid) const;

private:
	FileData CreateBinary(std::vector<Mesh::Vertex> && vertices, std::vector<uint32_t> && indices) const;
	std::vector<std::pair<std::vector<uint32_t>, std::vector<float>>> GetSkinning(const aiMesh* assimp_mesh, uint32_t mesh_skeleton_uuid) const;
};
#endif // !_MESHIMPORTER_H_