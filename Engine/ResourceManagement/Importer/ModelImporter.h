#ifndef _MODELIMPORTER_H_
#define _MODELIMPORTER_H_
#include "Importer.h"
#include "Helper/Timer.h"
#include <memory>

#include "assimp/LogStream.hpp"
#include "assimp/Logger.hpp"
#include "ModelImporters/MeshImporter.h"


struct aiNode;
struct aiScene;

class Mesh;
class ModelImporter : Importer
{
public:
	ModelImporter();
	~ModelImporter();
	std::pair<bool, std::string> Import(const File & file) const override;
	std::shared_ptr<Mesh> Load(const std::string& file_path) const;

	void RemoveMeshFromCacheIfNeeded(const std::shared_ptr<Mesh> & mesh);
private:
		void ImportNode(const aiNode* root_node, const aiMatrix4x4& parent_transformation, const aiScene* scene, const char* file_path, const std::string& output_file) const;

public:
	const float SCALE_FACTOR = 0.01f;

private:
	const std::string LIBRARY_MESHES_FOLDER = "Library/Meshes";
	mutable std::vector<std::shared_ptr<Mesh>> mesh_cache;
	mutable Timer performance_timer;
	std::unique_ptr<MeshImporter> meshImporter;
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
#endif // !_MODELIMPORTER_H_
