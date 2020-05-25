#include "Importer.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"

#include "ResourceManagement/Metafile/MetafileManager.h"
#include "ResourceManagement/ResourcesDB/CoreResources.h"

#include <pcg_basic.h>

Metafile* Importer::Import(Path& assets_file_path)
{
	Metafile* metafile;
	std::string metafile_path_string = App->resources->metafile_manager->GetMetafilePath(assets_file_path);

	if (App->filesystem->Exists(metafile_path_string))
	{
		metafile = App->resources->metafile_manager->GetMetafile(*App->filesystem->GetPath(metafile_path_string));
		if (metafile->version < Importer::IMPORTER_VERSION)
		{
			MetafileManager::UpdateMetafile(*metafile);
		}
	}
	else
	{
		if (core_resources_uuid_mapping.find(assets_file_path.GetFullPath()) != core_resources_uuid_mapping.end())
		{
			metafile = App->resources->metafile_manager->CreateMetafile(assets_file_path, resource_type, (uint32_t)core_resources_uuid_mapping[assets_file_path.GetFullPath()]);
		}
		else
		{
			metafile = App->resources->metafile_manager->CreateMetafile(assets_file_path, resource_type);
		}
	}

	std::string metafile_exported_folder = MetafileManager::GetMetafileExportedFolder(*metafile);
	if (!App->filesystem->Exists(metafile_exported_folder))
	{
		App->filesystem->MakeDirectory(metafile_exported_folder);
	}
	Path* metafile_exported_folder_path = App->filesystem->GetPath(metafile_exported_folder);

	FileData imported_data = ExtractData(assets_file_path, *metafile);
	metafile_exported_folder_path->Save(std::to_string(metafile->uuid).c_str(), imported_data);

	return metafile;
}

bool Importer::ImportRequired(const Path& file_path)
{
	std::string metafile_path_string = App->resources->metafile_manager->GetMetafilePath(file_path);
	if (App->filesystem->Exists(metafile_path_string))
	{
		Path* metafile_path = App->filesystem->GetPath(metafile_path_string);

		Metafile* metafile = App->resources->metafile_manager->GetMetafile(*metafile_path);
		assert(App->resources->metafile_manager->IsMetafileConsistent(*metafile));

		bool exported_file_exist = App->filesystem->Exists(metafile->exported_file_path);
		if (metafile->version < Importer::IMPORTER_VERSION || !exported_file_exist)
		{
			return true;
		}
		else if (exported_file_exist)
		{
			Path* library_path = App->filesystem->GetPath(metafile->exported_file_path);
			return  file_path.GetModificationTimestamp() > library_path->GetModificationTimestamp();
		}
		else
		{
			return false;
		}
	}

	return true;
}