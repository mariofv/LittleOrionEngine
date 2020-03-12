#ifndef _MODELIMPORTER_H_
#define _MODELIMPORTER_H_

#include "Importer.h"

#include "Helper/Timer.h"

#include <assimp/LogStream.hpp>
#include <assimp/Logger.hpp>
#include <memory>



struct aiNode;
struct aiScene;
class Config;
class Mesh;
class GameObject;
class MeshImporter;
class SkeletonImporter;
class AnimationImporter;


class ModelImporter : Importer
{
public:
	ModelImporter();
	~ModelImporter();
	ImportResult Import(const File & file, bool force = false) const override;

private:
	void ImportNode(const aiNode* root_node, const aiMatrix4x4& parent_transformation, const aiScene* scene, const char* file_path, const File& output_file, std::vector<Config> & node_config) const;

public:
	const float SCALE_FACTOR = 0.01f;

private:
	mutable Timer performance_timer;
	std::unique_ptr<MeshImporter> mesh_importer;
	std::unique_ptr<SkeletonImporter> skeleton_importer;
	std::unique_ptr<AnimationImporter> animation_importer;
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
