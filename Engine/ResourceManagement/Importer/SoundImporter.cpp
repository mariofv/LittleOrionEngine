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
	App->filesystem->Copy(soundbank_events_file_path, exported_events_directory, imported_events_file_name);


	return data;
}