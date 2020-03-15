#ifndef _MESHIMPORTER_H_
#define _MESHIMPORTER_H_

#include "ResourceManagement/Importer/Importer.h"
#include <vector>
#include <string>

#include <assimp/mesh.h>
class Mesh;
class MeshImporter : public Importer
{
public:
	MeshImporter() = default;
	~MeshImporter() = default;
	bool ImportMesh(const aiMesh* assimp_mesh, const aiMatrix4x4& mesh_transformation, const std::string& imported_file, std::string& exported_file) const;

private:
	void SaveBinary(const Mesh& own_format_mesh, const std::string& exported_file, const std::string& imported_file) const;

	const float SCALE_FACTOR = 0.01f;
};
#endif // !_MESHIMPORTER_H_