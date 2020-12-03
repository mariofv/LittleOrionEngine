#include "SoundManager.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Filesystem/PathAtlas.h"
#include "ResourceManagement/Metafile/MetafileManager.h"
#include "ResourceManagement/Resources/SoundBank.h"

static std::shared_ptr<SoundBank> init_sound_bank = nullptr;
std::shared_ptr<SoundBank> SoundManager::Init()
{
	//LOAD WWISE INIT
	Path* resource_exported_file_path = App->filesystem->GetPath(WWISE_INIT_PATH +std::string("/")+ WWISE_INIT_NAME);
	FileData exported_file_data = resource_exported_file_path->GetFile()->Load();
	std::shared_ptr<SoundBank> return_value = std::make_shared<SoundBank>(0, exported_file_data.buffer, exported_file_data.size);
	delete[] exported_file_data.buffer;
	return return_value;

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
	delete[] exported_file_data.buffer;
	
	std::string delimiter = "\n";
	size_t pos = 0;
	std::string event_name;
	std::vector<std::string> events;

	while ((pos = file_data.find(delimiter)) != std::string::npos) {
		event_name = file_data.substr(0, pos);
		file_data.erase(0, pos + delimiter.length());
		events.push_back(event_name);
	}

	return std::make_shared<SoundBank>(uuid, resource_data.buffer, resource_data.size, events);
}
