#include "Importer.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "Helper/Config.h"

std::pair<bool, std::string> Importer::Import(const File & file, bool force) const
{
	ImportOptions already_imported = GetAlreadyImportedResource(file);
	if (already_imported.uuid != 0 && !force) {
		return std::pair<bool, std::string>(true, already_imported.exported_file);
	}
	std::string exported_file = "Unknown";
	SaveMetaFile(file.file_path, ResourceType::UNKNOWN, exported_file);
	return std::pair<bool, std::string>(false, exported_file);
}


ImportOptions Importer::GetAlreadyImportedResource(const File & file_to_look_for) const
{
	std::string meta_file_path = GetMetaFilePath(file_to_look_for);

	File meta_file(meta_file_path);
	if (App->filesystem->Exists(meta_file_path.c_str()) && meta_file.modification_timestamp >= file_to_look_for.modification_timestamp) {
	
		ImportOptions options;
		GetOptionsFromMeta(meta_file,options);
		if (options.version != IMPORTER_VERSION) {
			options.uuid = 0;
			return options;
		}
		return options;
	}

	return ImportOptions();
}


void Importer::SaveMetaFile(const std::string& imported_path, ResourceType resource_type, const std::string& exported_path) const
{

	std::string meta_file_path = GetMetaFilePath(imported_path);

	Config scene_config;
	ImportOptions options;
	options.uuid = std::hash<std::string>{}(imported_path);
	options.resource_type = resource_type;
	options.version = IMPORTER_VERSION;
	options.exported_file = exported_path;
	options.imported_file = imported_path;
	options.Save(scene_config);

	std::string serialized_scene_string;
	scene_config.GetSerializedString(serialized_scene_string);

	App->filesystem->Save(meta_file_path.c_str(), serialized_scene_string.c_str(), serialized_scene_string.size() + 1);
	App->resources->resource_DB->AddEntry(options);
}


void Importer::GetOptionsFromMeta(const File& file, ImportOptions & options)
{
	size_t readed_bytes;
	char* meta_file_data = App->filesystem->Load(file.file_path.c_str(), readed_bytes);
	std::string serialized_string = meta_file_data;
	free(meta_file_data);

	Config meta_config(serialized_string);
	options.Load(meta_config);
}

std::string Importer::GetMetaFilePath(const File& file)
{
	return file.file_path + ".meta";
}