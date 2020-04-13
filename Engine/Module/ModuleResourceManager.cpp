#include "ModuleResourceManager.h"

#include "Helper/Config.h"
#include "Helper/Timer.h"
#include "ResourceManagement/Resources/Mesh.h"

#include <algorithm>
ModuleResourceManager::ModuleResourceManager()
{
	resource_DB = std::make_unique<ResourceDataBase>();
}

bool ModuleResourceManager::Init()
{
	APP_LOG_SECTION("************ Module Resource Manager Init ************");
	texture_importer = std::make_unique<TextureImporter>();
	material_importer = std::make_unique<MaterialImporter>();
	model_importer = std::make_unique<ModelImporter>();
	scene_manager = std::make_unique<SceneManager>();
	prefab_importer = std::make_unique<PrefabImporter>();

#if !GAME
	importing_thread = std::thread(&ModuleResourceManager::StartThread, this);
#else
	App->filesystem->MountDir("Library");
#endif

	File("Resources");
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
	 bool success = App->filesystem->CreateMountedDir("Library");
	 bool force_import = !App->filesystem->Exists("Library");
	 if (force_import && !success)
	 {
		 return;
	 }

	 thread_comunication.finished_loading = false;
	 thread_comunication.total_items = App->filesystem->assets_file->total_sub_files_number;
	 ImportAllFilesInDirectory(*App->filesystem->assets_file.get(), force_import);
	 thread_comunication.finished_loading = true;
	 last_imported_time = thread_timer->Read();
 }

 ImportResult ModuleResourceManager::Import(const File& file, bool force, bool skybox)
 {
	 while (thread_comunication.thread_importing_hash == std::hash<std::string>{}(file.file_path))
	 {
		 Sleep(1000);
	 } 
	 thread_comunication.main_importing_hash = std::hash<std::string>{}(file.file_path);
	 ImportResult  result = InternalImport(file, force, skybox);
	 thread_comunication.main_importing_hash = 0;
	 return result;
 }


 void ModuleResourceManager::CreatePrefab(const std::string& path, GameObject* gameobject_to_save) const
 {
	prefab_importer->CreatePrefabResource(File(path),gameobject_to_save);
 }

void ModuleResourceManager::ImportAllFilesInDirectory(const File& file, bool force)
 {
	 for (const auto& child : file.children)
	 {
		 if (thread_comunication.stop_thread)
		 {
			 return;
		 }
		 thread_comunication.thread_importing_hash = std::hash<std::string>{}(child->file_path);
		 while (thread_comunication.main_importing_hash == std::hash<std::string>{}(file.file_path))
		 {
			 Sleep(1000);
		 }
		 bool is_directory = child->file_type == FileType::DIRECTORY;
		 if (is_directory && !default_importer->Import(*child.get(), force).success)
		 {
			 ImportAllFilesInDirectory(*child.get(), force);
		 }
		 else if (!is_directory)
		 {
			 InternalImport(*child.get(), force);
		 }
		 ++thread_comunication.loaded_items;
		 thread_comunication.thread_importing_hash = 0;
	 }
 }


ImportResult ModuleResourceManager::InternalImport(const File& file, bool force, bool skybox) const
{
	ImportResult result;
	std::lock_guard<std::mutex> lock(thread_comunication.thread_mutex);
	if (file.file_type == FileType::MODEL)
	{
		result = model_importer->Import(file, force);
	}
	if (file.file_type == FileType::TEXTURE)
	{
		result = texture_importer->Import(file, force, skybox);
	}
	if (file.file_type == FileType::MATERIAL)
	{
		result = material_importer->Import(file, force);
	}
	if (file.file_type == FileType::PREFAB)
	{
		result = prefab_importer->Import(file, force);
	}
	if (file.file_type == FileType::MESH)
	{
		result = model_importer->ImportExtractedResources(file, force);
	}
	return result;
}

std::shared_ptr<Resource> ModuleResourceManager::RetrieveFromCacheIfExist(const std::string& uid) const
{
	if (!App->filesystem->Exists(uid.c_str()))
	{
		return nullptr;
	}
	//Check if the resource is already loaded
	auto& it = std::find_if(resource_cache.begin(), resource_cache.end(), [&uid](const std::shared_ptr<Resource> & resource)
	{
		return resource->exported_file == uid;
	});
	if (it != resource_cache.end())
	{
		APP_LOG_INFO("Resource %s exists in cache.", uid.c_str());
		return  *it;
	}
	return nullptr;
}

void  ModuleResourceManager::ReimportIfNeeded(const std::string& uid, bool skybox)
{
	if (std::find_if(uid.begin(), uid.end(), ::isdigit) == uid.end())
	{
		return;
	}
	std::string uid_string = uid.substr(uid.find_last_of("/") + 1, uid.size());
	uint32_t real_uuid = std::stoul(uid_string);
	const ImportOptions * options = resource_DB->GetEntry(real_uuid);
	if (options == nullptr)
	{
		return;
	}
	if (!App->filesystem->Exists(uid.c_str()) &&  App->filesystem->Exists(options->imported_file.c_str()))
	{
		Import(options->imported_file, true, skybox);
	}
}

uint32_t ModuleResourceManager::LoadCubemap(const std::vector<std::string>& faces_paths) 
{
	std::vector<std::string> faces_paths_dds;
	for (unsigned int i = 0; i < faces_paths.size(); i++)
	{
		std::string ol_texture = Import(File(faces_paths[i]), false, i!=2 && i!=3).exported_file;
		ReimportIfNeeded(ol_texture, i != 2 && i != 3);
		faces_paths_dds.push_back(ol_texture);
	}
	return TextureLoader::LoadCubemap(faces_paths_dds);
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
