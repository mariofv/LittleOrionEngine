#include "ModuleResourceManager.h"
#include "Main/Application.h"

#include "ModuleFileSystem.h"
#include "Helper/Config.h"
#include "Helper/Timer.h"
#include "ResourceManagement/Resources/Mesh.h"

bool ModuleResourceManager::Init()
{
	APP_LOG_SECTION("************ Module Resource Manager Init ************");
	texture_importer = std::make_unique<TextureImporter>();
	model_importer = std::make_unique<ModelImporter>();
	importing_thread = std::thread(&ModuleResourceManager::StartThread, this);
	thread_timer->Start();
	return true;
}

update_status ModuleResourceManager::PreUpdate()
{

	if ((thread_timer->Read() - last_imported_time) >= importer_interval_millis)
	{
		importing_thread.join();
		importing_thread = std::thread(&ModuleResourceManager::StartThread, this);
	}
	return update_status::UPDATE_CONTINUE;
}

 bool ModuleResourceManager::CleanUp()
{
	 thread_comunication.stop_thread = true;
	 importing_thread.join();
	return true;
}


 std::pair<bool, std::string> ModuleResourceManager::Import(const File& file)
 {
	 while (thread_comunication.thread_importing_hash == std::hash<std::string>{}(file.file_path))
	 {
		 Sleep(1000);
	 } 
	 thread_comunication.main_importing_hash = std::hash<std::string>{}(file.file_path);
	 std::pair<bool, std::string> result = InternalImport(file);
	 thread_comunication.main_importing_hash = 0;
	 return result;
 }


 void ModuleResourceManager::StartThread()
 {
	 thread_comunication.finished_loading = false;
	 thread_comunication.total_items = App->filesystem->assets_file->total_sub_files_number;
	 ImportAllFileHierarchy(*App->filesystem->assets_file.get());
	 thread_comunication.finished_loading = true;
	 last_imported_time = thread_timer->Read();
 }

void ModuleResourceManager::ImportAllFileHierarchy(const File& file)
 {

	 for (auto & child : file.children)
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
		 if (child->file_type == FileType::DIRECTORY && !default_importer->Import(*child.get()).first)
		 {
			 ImportAllFileHierarchy(*child.get());
		 }
		 else if (child->file_type != FileType::DIRECTORY)
		 {
			 InternalImport(*child.get());
		 }
		 ++thread_comunication.loaded_items;
		 thread_comunication.thread_importing_hash = 0;
	 }
 }


std::pair<bool, std::string> ModuleResourceManager::InternalImport(const File& file)
{
	std::pair<bool, std::string> result = std::pair<bool, std::string>(false,"");
	if (file.file_type == FileType::MODEL)
	{
		result = model_importer->Import(file);
	}
	if (file.file_type == FileType::TEXTURE)
	{
		std::lock_guard<std::mutex> lock(thread_comunication.thread_mutex);
		result = texture_importer->Import(file);
	}
	return result;
}

std::shared_ptr<Texture> ModuleResourceManager::LoadTexture(const std::string& file_path) const
{
	std::shared_ptr<Resource> resource = RetrieveFromCacheIfExist(file_path);
	if (resource != nullptr)
	{
		return std::static_pointer_cast<Texture>(resource);
	}
	std::shared_ptr<Texture> texture = texture_importer->Load(file_path);
	if (texture != nullptr)
	{
		resource_cache.push_back(texture);
	}
	return texture;
}

std::shared_ptr<Mesh> ModuleResourceManager::LoadModel(const std::string& file_path) const
{
	std::shared_ptr<Resource> resource = RetrieveFromCacheIfExist(file_path);
	if (resource != nullptr)
	{
		return std::static_pointer_cast<Mesh>(resource);
	}
	std::shared_ptr<Mesh> model = model_importer->Load(file_path);
	if (model != nullptr)
	{
		resource_cache.push_back(model);
	}
	return model;
}

void ModuleResourceManager::RemoveResourceFromCacheIfNeeded(const std::shared_ptr<Resource> & resource)
{
	auto it = std::find(resource_cache.begin(), resource_cache.end(), resource);
	if (it != resource_cache.end() && (*it).use_count() <= 2)
	{
		resource_cache.erase(it);
	}
}

std::shared_ptr<Resource> ModuleResourceManager::RetrieveFromCacheIfExist(const std::string& uid) const
{
	if (!App->filesystem->Exists(uid.c_str()))
	{
		return nullptr;
	}
	//Check if the resource is already loaded
	auto it = std::find_if(resource_cache.begin(), resource_cache.end(), [uid](const std::shared_ptr<Resource> resource)
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