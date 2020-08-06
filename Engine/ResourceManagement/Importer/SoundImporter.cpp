#include "SoundImporter.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Filesystem/PathAtlas.h"
#include "ResourceManagement/Metafile/Metafile.h"

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
	std::string soundbank_events_file_path = assets_file_path.GetFullPathWithoutExtension() + std::string(".txt");
	std::string imported_events_file_name = std::to_string(metafile.uuid) + std::string("_events");
	std::string exported_events_directory = metafile.exported_file_path.substr(0, metafile.exported_file_path.find_last_of("/"));

	FileData file_imported_data = App->filesystem->GetPath(soundbank_events_file_path)->GetFile()->Load();
	std::string file_data;
	file_data.assign(static_cast <const char*> (file_imported_data.buffer), file_imported_data.size);

	//Process the file '.txt' from Wwise to a file xxxxx_events which will contain only the names of the events of that soundbank
	file_data = file_data.substr(file_data.find("\n"), file_data.find("\r\n\r\n") - file_data.find("\n"));
	file_data.append("\r\n");

	std::string delimiter = "\t\r\n";
	size_t pos = 0;
	std::string token;
	std::string event_list;
	
	while ((pos = file_data.find(delimiter)) != std::string::npos) {
		token = file_data.substr(0, pos);
		event_list = event_list.append(token.substr(token.find_last_of("\\") +1, token.length()) + "\n");
		file_data.erase(0, pos + delimiter.length());
	}

	App->filesystem->Save(exported_events_directory + "/" + imported_events_file_name, event_list);


	return data;
}