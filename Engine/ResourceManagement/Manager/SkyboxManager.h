#ifndef _SKYBOXMANAGER_H_
#define _SKYBOXMANAGER_H_

#include <memory>
#include <string>

#include "Filesystem/File.h"

class Skybox;
class Metafile;
class Path;

class SkyboxManager
{
public:
	SkyboxManager() = default;
	~SkyboxManager() = default;

	static std::shared_ptr<Skybox> Load(Metafile* metafile, const FileData& resource_data);
	static uint32_t Create(Path& asset_creation_folder_path, const std::string created_asset_name= "New Skybox.skybox");
};

#endif // !_MATERIALMANAGER_H_



