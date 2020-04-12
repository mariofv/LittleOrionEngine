#ifndef _MATERIALMANAGER_H_
#define _MATERIALMANAGER_H_

#include <memory>
#include <string>

#include "Filesystem/File.h"

class Material;
class Metafile;
class Path;

class MaterialManager
{
public:
	MaterialManager() = default;
	~MaterialManager() = default;

	static std::shared_ptr<Material> Load(Metafile* metafile, const FileData& resource_data);
	static uint32_t Create(Path& asset_creation_folder_path, const std::string created_asset_name= "New Material.mat");
};

#endif // !_MATERIALMANAGER_H_



