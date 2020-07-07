#ifndef _MODELIMPORTER_H_
#define _MODELIMPORTER_H_

#include "ResourceManagement/Importer/Importer.h"

#include "Helper/Timer.h"

#include <assimp/LogStream.hpp>
#include <assimp/Logger.hpp>
#include <map>
#include <memory>

struct aiAnimation;
struct aiNode;
struct aiScene;
struct aiMesh;

class Config;
class Path;
class Metafile;

class ModelMetafile;

class ModelImporter : public Importer
{

struct CurrentModelData
{
	const aiScene* scene = nullptr;
	Path* asset_file_folder_path = nullptr;
	float scale = 1.f;
	std::map<std::string, uint32_t> skeleton_cache;
	 ModelMetafile* model_metafile;
	 bool remmaped_changed = false;
	 bool any_new_node = false;
	 bool animated_model = false;
};

public:
	ModelImporter();
	~ModelImporter();

	FileData ExtractData(Path& assets_file_path, const Metafile& metafile) const override;

private:
	std::vector<Config> ExtractDataFromNode(const aiNode* root_node, const aiMatrix4x4& parent_transformation) const;
	uint32_t ExtractMaterialFromNode(size_t mesh_index, const std::string& mesh_name) const;
    uint32_t SaveDataInLibrary(Metafile &node_metafile, FileData &mesh_material_data) const;
	uint32_t ExtractMeshFromNode(const aiMesh* asssimp_mesh, std::string mesh_name, const aiMatrix4x4& mesh_transformation, uint32_t mesh_skeleton_uuid) const;
	uint32_t ExtractSkeletonFromNode(const aiMesh* asssimp_mesh, std::string mesh_name) const;
	uint32_t ExtractAnimationFromNode(const aiAnimation* assimp_animation, std::string animation_name) const;

private:
	mutable Timer performance_timer;
	mutable CurrentModelData current_model_data;
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
			MYASSIMP_LOG_DEBUG("%s", message);
			break;
		case Assimp::Logger::Info:
			MYASSIMP_LOG_INFO("%s", message);
			break;
		case Assimp::Logger::Err:
			MYASSIMP_LOG_ERROR("%s", message);
			break;
		case Assimp::Logger::Warn:
			MYASSIMP_LOG_WARNING("%s", message); 
			break;

		}
	}

public:
	unsigned int severety = 0;
	
};
#endif // !_MODELIMPORTER_H_
