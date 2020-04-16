#include "MetafileManager.h"

#include "Filesystem/Path.h"
#include "Filesystem/PathAtlas.h"
#include "Helper/Config.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Metafile.h"
#include "ResourceManagement/Importer/Importer.h"
#include "ResourceManagement/Resources/Resource.h"

#include <pcg_basic.h>
#include <chrono>

MetafileManager::~MetafileManager()
{
	for (auto& metafile : metafiles)
	{
		delete metafile.second;
	}
	metafiles.clear();
}

Metafile* MetafileManager::GetMetafile(const Path& metafile_path)
{
	if (metafiles.find(metafile_path.GetFullPath()) != metafiles.end())
	{
		return metafiles[metafile_path.GetFullPath()];
	}

	FileData meta_file_data = metafile_path.GetFile()->Load();
	std::string serialized_string ((char*)meta_file_data.buffer, meta_file_data.size);
	free((char*)meta_file_data.buffer);

	Metafile* created_metafile = new Metafile();
	created_metafile->metafile_path = metafile_path.GetFullPath();
	Config meta_config(serialized_string);
	created_metafile->Load(meta_config);

	metafiles[created_metafile->metafile_path] = created_metafile;

	return created_metafile;
}


Metafile* MetafileManager::CreateMetafile(const std::string& asset_file_path_string, ResourceType resource_type)
{
	return CreateMetafile(*App->filesystem->GetPath(asset_file_path_string), resource_type);
}

Metafile* MetafileManager::CreateMetafile(Path& asset_file_path, ResourceType resource_type, uint32_t uuid)
{

	Metafile* created_metafile = new Metafile();

	std::string metafile_path_string = GetMetafilePath(asset_file_path);
	assert(metafiles.find(metafile_path_string) == metafiles.end());

	created_metafile->uuid = uuid == 0 ? pcg32_random() : uuid;
	created_metafile->resource_name = asset_file_path.GetFilenameWithoutExtension();
	created_metafile->resource_type = resource_type;

	created_metafile->metafile_path = metafile_path_string;
	created_metafile->imported_file_path = asset_file_path.GetFullPath();
	created_metafile->exported_file_path = GetMetafileExportedFile(*created_metafile);

	created_metafile->version = Importer::IMPORTER_VERSION;

	Config metafile_config;
	created_metafile->Save(metafile_config);

	std::string metafile_config_string;
	metafile_config.GetSerializedString(metafile_config_string);

	std::string metfile_name_string = GetMetafilePath(asset_file_path.GetFilename());
	asset_file_path.GetParent()->Save(metfile_name_string.c_str(), metafile_config_string);

	metafiles[created_metafile->metafile_path] = created_metafile;

	return created_metafile;
}

std::string MetafileManager::GetMetafilePath(const Path& file_path)
{
	return GetMetafilePath(file_path.GetFullPath());
}

std::string MetafileManager::GetMetafilePath(const std::string& file_path_string)
{
	return file_path_string + ".meta";
}

void MetafileManager::TouchMetafileTimestamp(Metafile& metafile)
{
	using namespace std::chrono;
	seconds current_timestamp = duration_cast<seconds>(system_clock::now().time_since_epoch());
	long long timestamp = current_timestamp.count();
	metafile.timestamp = timestamp;

	Config metafile_config;
	metafile.Save(metafile_config);

	std::string metafile_config_string;
	metafile_config.GetSerializedString(metafile_config_string);

	Path* metafile_path = App->filesystem->GetPath(metafile.metafile_path);
	metafile_path->GetParent()->Save(metafile_path->GetFilename().c_str(), metafile_config_string);
}

bool MetafileManager::IsMetafileConsistent(const Path& metafile_path)
{
	Metafile* metafile = GetMetafile(metafile_path);
	return IsMetafileConsistent(*metafile);
}

bool MetafileManager::IsMetafileConsistent(const Metafile& metafile)
{
	return App->filesystem->Exists(metafile.imported_file_path);
}

void MetafileManager::DeleteMetafileInconsistencies(const Path& metafile_path)
{
	Metafile* metafile = GetMetafile(metafile_path);
	DeleteMetafileInconsistencies(*metafile);
}

void MetafileManager::DeleteMetafileInconsistencies(const Metafile& metafile)
{
	if (App->filesystem->Exists(metafile.exported_file_path))
	{
		App->filesystem->Remove(metafile.exported_file_path);
	}

	App->filesystem->Remove(metafile.metafile_path);
}

std::string MetafileManager::GetMetafileExportedFolder(const Metafile& metafile)
{
	std::string uuid_string = std::to_string(metafile.uuid);
	return std::string(LIBRARY_METADATA_PATH) + "/" + uuid_string.substr(0, 2);
}

std::string MetafileManager::GetMetafileExportedFile(const Metafile& metafile)
{
	std::string uuid_string = std::to_string(metafile.uuid);
	return std::string(LIBRARY_METADATA_PATH) + "/" + uuid_string.substr(0, 2) + "/" + uuid_string;
}