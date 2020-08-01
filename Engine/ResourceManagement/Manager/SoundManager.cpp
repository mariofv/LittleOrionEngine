#include "SoundManager.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Filesystem/PathAtlas.h"
#include "ResourceManagement/Resources/SoundBank.h"

static std::shared_ptr<SoundBank> init_sound_bank = nullptr;
std::shared_ptr<SoundBank> SoundManager::Init()
{
	//LOAD WWISE INIT
	Path* resource_exported_file_path = App->filesystem->GetPath(WWISE_INIT_PATH +std::string("/")+ WWISE_INIT_NAME);
	FileData exported_file_data = resource_exported_file_path->GetFile()->Load();
	return std::make_shared<SoundBank>(0, exported_file_data.buffer, exported_file_data.size);
}

std::shared_ptr<SoundBank> SoundManager::Load(uint32_t uuid, const FileData & resource_data)
{
	if (init_sound_bank == nullptr)
	{
		init_sound_bank = SoundManager::Init();
	}
	return std::make_shared<SoundBank>(uuid, resource_data.buffer, resource_data.size);
}
