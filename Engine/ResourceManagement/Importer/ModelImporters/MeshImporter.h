#ifndef _MESHIMPORTER_H_
#define _MESHIMPORTER_H_
#include <vector>
#include <string>

#include <assimp/mesh.h>
class MeshImporter
{
public:
	MeshImporter() = default;
	~MeshImporter() = default;
	bool ImportMesh(const aiMesh* file_path, const aiMatrix4x4& mesh_transformation, const std::string& output_file) const;
};
#endif // !_MESHIMPORTER_H_