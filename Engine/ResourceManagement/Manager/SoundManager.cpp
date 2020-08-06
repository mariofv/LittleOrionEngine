#include "SoundManager.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "ResourceManagement/Metafile/MetafileManager.h"
#include "Filesystem/PathAtlas.h"
#include "ResourceManagement/Resources/SoundBank.h"

static std::shared_ptr<SoundBank> init_sound_bank = nullptr;
std::shared_ptr<SoundBank>  SoundManager::Init()
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

	std::string exported_filepath = MetafileManager::GetUUIDExportedFile(uuid) + std::string("_events");
	Path* resource_exported_file_path = App->filesystem->GetPath(exported_filepath);
	FileData exported_file_data = resource_exported_file_path->GetFile()->Load();
	std::string file_data;
	file_data.assign(static_cast <const char*> (exported_file_data.buffer), exported_file_data.size);

	//TODO:Read the names of the events and asssign them to the vector<string> events which will be an attribute of the soundbank.
	//
	//
	//
	//Free the Filedata
	std::vector<std::string> events;

	return std::make_shared<SoundBank>(uuid, resource_data.buffer, resource_data.size, events);
}
