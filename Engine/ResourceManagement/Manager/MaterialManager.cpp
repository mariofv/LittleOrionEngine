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

FileData MaterialManager::Binarize(Material* material)
{
	Config material_config;
	material->Save(material_config);

	std::string serialized_material_string;
	material_config.GetSerializedString(serialized_material_string);

	char* material_bytes = new char[serialized_material_string.size() + 1];
	memcpy(material_bytes, serialized_material_string.c_str(), serialized_material_string.size() + 1);

	FileData material_data{ material_bytes, serialized_material_string.size() + 1 };
	return material_data;
}

std::shared_ptr<Material> MaterialManager::Load(uint32_t uuid, const FileData& resource_data)
{
	char* material_file_data = (char*)resource_data.buffer;

	std::string serialized_material_string = std::string(material_file_data, resource_data.size);

	Config material_config(serialized_material_string);
	std::shared_ptr<Material> new_material = std::make_shared<Material>(uuid);
	new_material->Load(material_config);

	return new_material;
}

FileData MaterialManager::Create()
{
	Material material;
	return Binarize(&material);
}