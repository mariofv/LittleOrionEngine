#include "SoundManager.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Filesystem/PathAtlas.h"
#include "ResourceManagement/Resources/Sound.h"

std::shared_ptr<Sound>  SoundManager::Init()
{
	//LOAD WWISE INIT
	Path* resource_exported_file_path = App->filesystem->GetPath(WWISE_INIT_PATH);
	FileData exported_file_data = resource_exported_file_path->GetFile()->Load();
	return std::make_shared<Sound>(0, exported_file_data.buffer, exported_file_data.size);
}

std::shared_ptr<Sound> SoundManager::Load(uint32_t uuid, const FileData & resource_data)
{
	return std::make_shared<Sound>(uuid, resource_data.buffer, resource_data.size);
}
