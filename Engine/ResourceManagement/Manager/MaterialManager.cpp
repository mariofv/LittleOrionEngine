#include "MaterialManager.h"

#include "Helper/Config.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Metafile/Metafile.h"
#include "ResourceManagement/Resources/Material.h"
#include "ResourceManagement/Resources/Texture.h"

#include <Brofiler/Brofiler.h>
#include <vector>

std::shared_ptr<Material> MaterialManager::Load(Metafile* metafile, const FileData& resource_data)
{
	char* material_file_data = (char*)resource_data.buffer;

	std::string serialized_material_string = std::string(material_file_data, resource_data.size);

	Config material_config(serialized_material_string);
	std::shared_ptr<Material> new_material = std::make_shared<Material>(metafile);
	new_material->Load(material_config);

	return new_material;
}


uint32_t MaterialManager::Create(Path& asset_creation_folder_path, const std::string created_asset_name)
{
	Material material;
	Config material_config;
	material.Save(material_config);
	
	std::string serialized_material_string;
	material_config.GetSerializedString(serialized_material_string);

	char* material_bytes = new char[serialized_material_string.size() + 1];
	memcpy(material_bytes, serialized_material_string.c_str(), serialized_material_string.size() + 1);

	FileData material_data{ material_bytes, serialized_material_string.size() + 1 };
	return App->resources->CreateFromData(material_data, asset_creation_folder_path, created_asset_name);
}