#ifndef _MODELIMPORTER_H_
#define _MODELIMPORTER_H_

#include "ResourceManagement/Importer/Importer.h"

#include "Helper/Timer.h"

#include <assimp/LogStream.hpp>
#include <assimp/Logger.hpp>
#include <memory>

struct aiAnimation;
struct aiNode;
struct aiScene;
struct aiMesh;

class Config;
class Path;

class ModelImporter : public Importer
{
public:
	ModelImporter();
	~ModelImporter();

	FileData ExtractData(Path& assets_file_path) const override;

private:
	std::vector<Config> ExtractDataFromNode(const aiNode* root_node, const aiMatrix4x4& parent_transformation, const aiScene* scene, Path& asset_file_folder_path) const;
	uint32_t ExtractMaterialFromNode(const aiScene* scene, size_t mesh_index, const std::string& mesh_name, Path& asset_file_folder_path) const;
	uint32_t ExtractMeshFromNode(const aiMesh* asssimp_mesh, std::string mesh_name, const aiMatrix4x4& mesh_transformation, Path& asset_file_folder_path) const;
	uint32_t ExtractSkeletonFromNode(const aiScene* scene, const aiMesh* asssimp_mesh, std::string mesh_name, Path& asset_file_folder_path) const;
	uint32_t ExtractAnimationFromNode(const aiScene* scene, const aiAnimation* assimp_animation, std::string animation_name, Path& asset_file_folder_path) const;

private:
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
#endif // !_MODELIMPORTER_H_
