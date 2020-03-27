#include "SkyboxManager.h"

#include "Helper/Config.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"

#include "ResourceManagement/Manager/TextureManager.h"
#include "ResourceManagement/Metafile/Metafile.h"
#include "ResourceManagement/Resources/Skybox.h"

std::shared_ptr<Skybox> SkyboxManager::Load(Metafile* metafile, const FileData& resource_data)
{
	char* material_file_data = (char*)resource_data.buffer;

	std::string serialized_material_string = std::string(material_file_data, resource_data.size);

	Config material_config(serialized_material_string);
	std::shared_ptr<Skybox> new_skybox = std::make_shared<Skybox>(metafile);
	new_skybox->Load(material_config);

	return new_skybox;
}

uint32_t SkyboxManager::Create(Path& asset_creation_folder_path, const std::string created_asset_name)
{
	Skybox skybox;
	Config skybox_config;
	skybox.Save(skybox_config);
	
	std::string serialized_skybox_string;
	skybox_config.GetSerializedString(serialized_skybox_string);

	char* skybox_bytes = new char[serialized_skybox_string.size() + 1];
	memcpy(skybox_bytes, serialized_skybox_string.c_str(), serialized_skybox_string.size() + 1);

	FileData skybox_data{ skybox_bytes, serialized_skybox_string.size() + 1 };
	return App->resources->CreateFromData(skybox_data, asset_creation_folder_path, created_asset_name);
}