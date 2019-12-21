#ifndef _MESHIMPORTER_H_
#define _MESHIMPORTER_H_
#include "Importer.h"
#include "Timer.h"
#include <memory>

#include "assimp/LogStream.hpp"
#include "assimp/Logger.hpp"

struct aiMesh;
struct aiNode;
struct aiScene;

class Mesh;
class MeshImporter : Importer
{
public:
	MeshImporter();
	~MeshImporter();
	bool Import(const char* file_path, std::string& output_file) const override;
	std::shared_ptr<Mesh> Load(const char* file_path) const;

	void RemoveMeshFromCacheIfNeeded(std::shared_ptr<Mesh>);
private:
	void ImportMesh(const aiMesh* file_path, const std::vector<std::string> & loaded_meshes_materials, const std::string& output_file) const;
	void ImportNode(const aiNode * root_node, const aiScene* scene, const char* file_path,const std::string& output_file) const;

private:
	const std::string LIBRARY_MESHES_FOLDER = ".//Library//Meshes";
	mutable std::vector<std::shared_ptr<Mesh>> mesh_cache;
	mutable Timer performance_timer;
};


class AssimpStream : public Assimp::LogStream
{
public:
	AssimpStream() = default;
	AssimpStream(const unsigned int severety) :severety(severety) {}
	~AssimpStream() = default;

	void write(const char* message)
	{
		switch (severety)
		{
		case Assimp::Logger::Debugging:
			MYASSIMP_LOG_INFO("%s", message);
			break;
		case Assimp::Logger::Info:
			MYASSIMP_LOG_INFO("%s", message);
			break;
		case Assimp::Logger::Err:
			MYASSIMP_LOG_ERROR("%s", message);
			break;
		case Assimp::Logger::Warn:
			MYASSIMP_LOG_INIT("%s", message); // Actually not an itialization entry, I use this type of entry because the yellow color
			break;

		}
	}

public:
	unsigned int severety = 0;
};
#endif // !_MESHIMPORTER_H_
