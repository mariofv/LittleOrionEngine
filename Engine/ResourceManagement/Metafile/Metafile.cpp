#include "Metafile.h"

#include "Helper/Config.h"

void Metafile::Save(Config& config) const
{
	config.AddUInt(uuid, "UUID");
	config.AddString(resource_name, "ResourceName");
	config.AddUInt(static_cast<unsigned int>(resource_type), "ResourceType");

	config.AddString(metafile_path, "MetfilePath");
	config.AddString(imported_file_path, "ImportedFilePath");
	config.AddString(exported_file_path, "ExportedFilePath");

	config.AddInt64(timestamp, "Timestamp");
	config.AddInt(version, "ImporterVersion");
}

void Metafile::Load(const Config& config)
{
	uuid = config.GetUInt("UUID", uuid);
	config.GetString("ResourceName", resource_name, "");
	resource_type = static_cast<ResourceType>(config.GetUInt("ResourceType",  static_cast<unsigned int>(ResourceType::UNKNOWN)));

	config.GetString("MetfilePath", metafile_path, "");
	config.GetString("ImportedFilePath", imported_file_path, "");
	config.GetString("ExportedFilePath", exported_file_path, "");

	timestamp = config.GetInt64("Timestamp", 0);
	version = config.GetInt("ImporterVersion", 0);
}