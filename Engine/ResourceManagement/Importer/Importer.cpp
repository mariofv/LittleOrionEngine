#include "Importer.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "Helper/Config.h"
#include <pcg_basic.h>

ImportResult Importer::Import(const File & file, bool force) const
{
	ImportResult import_result;

	ImportOptions already_imported = GetAlreadyImportedResource(file);
	if (already_imported.uuid != 0 && !force) {
		import_result.succes = true;
		import_result.exported_file = already_imported.exported_file;
		return import_result;
	}
	std::string exported_file = SaveMetaFile(file.file_path, ResourceType::UNKNOWN);

	return import_result;
}


ImportOptions Importer::GetAlreadyImportedResource(const File & file_to_look_for) const
{
	std::string meta_file_path = GetMetaFilePath(file_to_look_for);

	File meta_file(meta_file_path);
	if (App->filesystem->Exists(meta_file_path.c_str()) && meta_file.modification_timestamp >= file_to_look_for.modification_timestamp) {
	
		ImportOptions options;
		GetOptionsFromMeta(meta_file,options);
		if (options.version != IMPORTER_VERSION || options.timestamp < file_to_look_for.modification_timestamp) {
			options.uuid = 0;
			return options;
		}
		return options;
	}

	return ImportOptions();
}


std::string Importer::SaveMetaFile(const std::string& imported_path, ResourceType resource_type) const
{
	std::string meta_file_path = GetMetaFilePath(imported_path);

	Config scene_config;
	ImportOptions options;
	if (App->filesystem->Exists(meta_file_path.c_str()))
	{
		ImportOptions old_options;
		GetOptionsFromMeta(meta_file_path,old_options);
		options.uuid = old_options.uuid;
	}
	else
	{
		options.uuid = pcg32_random();
	}

	std::string uuid_string = std::to_string(options.uuid);
	std::string exported_path = LIBRARY_METADATA_PATH + "/" + uuid_string.substr(0, 2);
	App->filesystem->MakeDirectory(exported_path);
	options.exported_file = exported_path + "/" + uuid_string;
	options.resource_type = resource_type;
	options.version = IMPORTER_VERSION;
	options.imported_file = imported_path;
	options.Save(scene_config);

	std::string serialized_scene_string;
	scene_config.GetSerializedString(serialized_scene_string);

	App->filesystem->Save(meta_file_path.c_str(), serialized_scene_string.c_str(), serialized_scene_string.size() + 1);
	App->resources->resource_DB->AddEntry(options);
	return options.exported_file;
}


void Importer::GetOptionsFromMeta(const File& file, ImportOptions & options)
{
	if (!App->filesystem->Exists(file.file_path.c_str()))
	{
		return;
	}
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