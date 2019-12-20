#ifndef _MESHIMPORTER_H_
#define _MESHIMPORTER_H_
#include "Importer.h"
#include "Timer.h"
#include <memory>

class aiMesh;
class aiNode;
class aiScene;
class Mesh;
class MeshImporter : Importer
{
public:
	MeshImporter();
	bool Import(const char* file_path, std::string& output_file) override;
	std::shared_ptr<Mesh> Load(const char* file_path);

	void RemoveMeshFromCacheIfNeeded(std::shared_ptr<Mesh>);
private:
	void ImportMesh(const aiMesh* file_path, const std::string& output_file) const;
	void ImportNode(const aiNode * root_node, const aiScene* scene, const std::string& output_file);
	std::shared_ptr<ModuleFileSystem::File> GetAlreadyImportedMesh(const ModuleFileSystem::File & file) const;

private:
	const std::string LIBRARY_MESHES_FOLDER = ".//Library//Meshes";
	Timer performance_timer;
	std::vector<std::shared_ptr<Mesh>> mesh_cache;
};
#endif // !_MESHIMPORTER_H_
