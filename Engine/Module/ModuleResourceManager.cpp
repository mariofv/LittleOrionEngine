#include "ModuleResourceManager.h"

#include "Filesystem/PathAtlas.h"

#include "Helper/Config.h"
#include "Helper/Timer.h"

#include "Main/GameObject.h"

#include "ResourceManagement/Importer/Importer.h"
#include "ResourceManagement/Importer/MaterialImporter.h"
#include "ResourceManagement/Importer/ModelImporter.h"
#include "ResourceManagement/Importer/ModelImporters/AnimationImporter.h"
#include "ResourceManagement/Importer/ModelImporters/MeshImporter.h"
#include "ResourceManagement/Importer/ModelImporters/SkeletonImporter.h"
#include "ResourceManagement/Importer/PrefabImporter.h"
#include "ResourceManagement/Importer/SkyboxImporter.h"
#include "ResourceManagement/Importer/StateMachineImporter.h"
#include "ResourceManagement/Importer/TextureImporter.h"

#include "ResourceManagement/Manager/AnimationManager.h"
#include "ResourceManagement/Manager/MaterialManager.h"
#include "ResourceManagement/Manager/MeshManager.h"
#include "ResourceManagement/Manager/PrefabManager.h"
#include "ResourceManagement/Manager/SceneManager.h"
#include "ResourceManagement/Manager/SkeletonManager.h"
#include "ResourceManagement/Manager/SkyboxManager.h"
#include "ResourceManagement/Manager/StateMachineManager.h"
#include "ResourceManagement/Manager/TextureManager.h"

#include "ResourceManagement/Metafile/Metafile.h"
#include "ResourceManagement/Metafile/MetafileManager.h"

#include <algorithm>
#include <functional> //for std::hash

ModuleResourceManager::ModuleResourceManager()
{
	resource_DB = std::make_unique<ResourceDataBase>();
}

bool ModuleResourceManager::Init()
{
	APP_LOG_SECTION("************ Module Resource Manager Init ************");

	animation_importer = std::make_unique<AnimationImporter>();
	material_importer = std::make_unique<MaterialImporter>();
	mesh_importer = std::make_unique<MeshImporter>();
	model_importer = std::make_unique<ModelImporter>();
	prefab_importer = std::make_unique<PrefabImporter>();
	skeleton_importer = std::make_unique<SkeletonImporter>();
	skybox_importer = std::make_unique<SkyboxImporter>();
	state_machine_importer = std::make_unique<StateMachineImporter>();
	texture_importer = std::make_unique<TextureImporter>();

	metafile_manager = std::make_unique<MetafileManager>();
	scene_manager = std::make_unique<SceneManager>();

#if !GAME
	ImportAssetsInDirectory(*App->filesystem->resources_folder_path); // Import all assets in folder Resources. All metafiles in Resources are correct"
	importing_thread = std::thread(&ModuleResourceManager::StartThread, this);
#else
	App->filesystem->MountDir("Library");
#endif

	thread_timer->Start();
	return true;
}

update_status ModuleResourceManager::PreUpdate()
{
	if ((thread_timer->Read() - last_imported_time) >= importer_interval_millis)
	{
		//importing_thread.join();
		//importing_thread = std::thread(&ModuleResourceManager::StartThread, this);
		CleanResourceCache();
	}
	return update_status::UPDATE_CONTINUE;
}

bool ModuleResourceManager::CleanUp()
{
#if !GAME
	 thread_comunication.stop_thread = true;
	 importing_thread.join();
#endif
	return true;
}

 void ModuleResourceManager::StartThread()
 {
	 thread_comunication.finished_loading = false;
	 thread_comunication.total_items = App->filesystem->assets_folder_path->total_sub_files_number;

	 CleanInconsistenciesInDirectory(*App->filesystem->assets_folder_path); // Clean all incorrect meta in the folder Assets.
	 ImportAssetsInDirectory(*App->filesystem->assets_folder_path); // Import all assets in folder Assets. All metafiles in Assets are correct"
	// TODO: We need also to iterate over the library folder and to delete binaries that doesnt have corresponding metafiles
	 thread_comunication.finished_loading = true;
	 last_imported_time = thread_timer->Read();
 }

void ModuleResourceManager::CleanInconsistenciesInDirectory(const Path& directory_path)
{
	for (auto & path_child : directory_path.children)
	{
		if (thread_comunication.stop_thread)
		{
			return;
		}
		thread_comunication.thread_importing_hash = std::hash<std::string>{}(path_child->GetFullPath());
		while (thread_comunication.main_importing_hash == std::hash<std::string>{}(directory_path.GetFullPath()))
		{
			Sleep(1000);
		}

		if (path_child->IsDirectory())
		{
			CleanInconsistenciesInDirectory(*path_child);
		}
		else if (path_child->IsMeta())
		{
			if (!metafile_manager->IsMetafileConsistent(*path_child))
			{
				metafile_manager->DeleteMetafileInconsistencies(*path_child); // TODO: This causes memory violations because bector size is modified. Look for a better way of deleting files.
			}
		}
		thread_comunication.thread_importing_hash = 0;
	}
}

void ModuleResourceManager::ImportAssetsInDirectory(const Path& directory_path)
 {
	 for (size_t i = 0; i < directory_path.children.size(); ++i)
	 {
		 Path* path_child = directory_path.children[i];
		 if (thread_comunication.stop_thread)
		 {
			 return;
		 }
		 /*
		 thread_comunication.thread_importing_hash = std::hash<std::string>{}(path_child->GetFullPath());
		 while (thread_comunication.main_importing_hash == std::hash<std::string>{}(directory_path.GetFullPath()))
		 {
			 Sleep(1000);
		 }
		 */
		 if (path_child->IsDirectory())
		 {
			 ImportAssetsInDirectory(*path_child);
		 }
		 else if (path_child->IsImportable())
		 {
			 Import(*path_child);
			 ++thread_comunication.loaded_items;
		 }
		 thread_comunication.thread_importing_hash = 0;
	 }
 }

uint32_t ModuleResourceManager::Import(Path& file_path)
{
	std::lock_guard<std::mutex> lock(thread_comunication.thread_mutex);
	uint32_t imported_resource_uuid = InternalImport(file_path);
	return imported_resource_uuid;
}

uint32_t ModuleResourceManager::InternalImport(Path& file_path) const
{
	Metafile* asset_metafile = nullptr;

	if (Importer::ImportRequired(file_path))
	{
		switch (file_path.GetFile()->GetFileType())
		{
		case FileType::ANIMATION:
			asset_metafile = animation_importer->Import(file_path);
			break;
		
		case FileType::MATERIAL:
			asset_metafile = material_importer->Import(file_path);
			break;

		case FileType::MESH:
			asset_metafile = mesh_importer->Import(file_path);
			break;

		case FileType::MODEL:
			asset_metafile = model_importer->Import(file_path); // WARNING! If you load a model dont forget to put Prefab overwritable flag to false!!
			break;

		case FileType::PREFAB:
			asset_metafile = prefab_importer->Import(file_path);
			break;

		case FileType::SKELETON:
			asset_metafile = skeleton_importer->Import(file_path);
			break;

		case FileType::SKYBOX:
			asset_metafile = skybox_importer->Import(file_path);
			break;

		case FileType::STATE_MACHINE:
			asset_metafile = state_machine_importer->Import(file_path);
			break;

		case FileType::TEXTURE:
			asset_metafile = texture_importer->Import(file_path);
			break;
		}
	}
	else
	{
		Path* metafile_path = App->filesystem->GetPath(metafile_manager->GetMetafilePath(file_path));
		asset_metafile = metafile_manager->GetMetafile(*metafile_path);
	}

	resource_DB->AddEntry(asset_metafile);

	return asset_metafile->uuid;
}

uint32_t ModuleResourceManager::CreateFromData(FileData data, Path& creation_folder_path, const std::string& created_resource_name)
{
	Path* created_asset_file_path = creation_folder_path.Save(created_resource_name.c_str(), data);
	return App->resources->InternalImport(*created_asset_file_path);
}

std::shared_ptr<Resource> ModuleResourceManager::RetrieveFromCacheIfExist(uint32_t uuid) const
{
	//Check if the resource is already loaded
	auto& it = std::find_if(resource_cache.begin(), resource_cache.end(), [&uuid](const std::shared_ptr<Resource> & resource)
	{
		return resource->GetUUID() == uuid;
	});

	if (it != resource_cache.end())
	{
		APP_LOG_INFO("Resource %u exists in cache.", uuid);
		return *it;
	}
	return nullptr;
}

void ModuleResourceManager::CleanResourceCache()
{
	const auto it = std::remove_if(resource_cache.begin(), resource_cache.end(), [](const std::shared_ptr<Resource> & resource) {
		return resource.use_count() == 1;
	});
	if (it != resource_cache.end())
	{
		resource_cache.erase(it, resource_cache.end());
	}
}