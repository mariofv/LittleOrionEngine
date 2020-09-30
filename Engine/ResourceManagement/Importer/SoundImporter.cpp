#include "SoundImporter.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Filesystem/PathAtlas.h"
#include "ResourceManagement/Metafile/Metafile.h"

#include <rapidxml-1.13/rapidxml.hpp>

FileData SoundImporter::ExtractData(Path& assets_file_path, const Metafile& metafile) const
{
	FileData data = assets_file_path.GetFile()->Load();

	if (assets_file_path.GetFilename() == WWISE_INIT_NAME)
	{
		Metafile& casted_metafile = const_cast<Metafile&>(metafile);
		casted_metafile.exported_file_path = WWISE_INIT_PATH +std::string("/") + WWISE_INIT_NAME;
		casted_metafile.uuid = 0;

		Path* setup_exported_folder_path = App->filesystem->MakeDirectory(WWISE_INIT_PATH);
		setup_exported_folder_path->Save(WWISE_INIT_NAME, data);
		return { nullptr,0 };

	}	
	
	if (soundbank_events.size == 0)
	{
		GenerateSoundBankEventsInfo();
	}

	std::string soundbank_name = assets_file_path.GetFilenameWithoutExtension();

	if (soundbank_events.find(soundbank_name) == soundbank_events.end())
	{
		soundbank_events[soundbank_name] = std::vector<std::string>();
	}
	std::vector<std::string> event_list = soundbank_events[soundbank_name];

	std::string exported_events_directory = metafile.exported_file_path.substr(0, metafile.exported_file_path.find_last_of("/"));
	std::string exported_events_file_name = std::to_string(metafile.uuid) + std::string("_events");

	//App->filesystem->Save(exported_events_directory + "/" + exported_events_file_name, event_list);

	return data;
}

void SoundImporter::GenerateSoundBankEventsInfo() const
{
	FileData file_imported_data = App->filesystem->GetPath(SOUNDBANKS_XML_PATH)->GetFile()->Load();
	std::string file_data;
	file_data.assign(static_cast <const char*> (file_imported_data.buffer), file_imported_data.size);
	free((void*)file_imported_data.buffer);

}