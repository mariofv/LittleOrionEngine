#ifndef _MESHIMPORTER_H_
#define _MESHIMPORTER_H_

#include "ResourceManagement/Importer/Importer.h"
#include <ResourceManagement/Resources/Mesh.h>
#include <vector>
#include <string>

#include <assimp/mesh.h>
class MeshImporter : public Importer
{
public:
	MeshImporter() = default;
	~MeshImporter() = default;
	ImportResult Import(const File & file, bool force = false) const;
	bool ImportMesh(const aiMesh* assimp_mesh, const aiMatrix4x4& mesh_transformation, const std::string& imported_file, std::string& exported_file) const;

private:
	void SaveBinary(std::vector<Mesh::Vertex> && vertices, std::vector<uint32_t> && indices, const std::string& exported_file, const std::string& imported_file) const;
};
#endif // !_MESHIMPORTER_H_