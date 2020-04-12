#ifndef _MODULERESOURCEMANAGER_H_
#define _MODULERESOURCEMANAGER_H_

#include "Main/Application.h"
#include "Module.h"
#include "ModuleFileSystem.h"

#include "ResourceManagement/Importer/Importer.h"
#include "ResourceManagement/Importer/MaterialImporter.h"
#include "ResourceManagement/Importer/ModelImporter.h"
#include "ResourceManagement/Importer/PrefabImporter.h"
#include "ResourceManagement/Importer/SceneManager.h"
#include "ResourceManagement/Importer/TextureImporter.h"
#include "ResourceManagement/ResourcesDB/ResourceDataBase.h"

#include <memory>
#include <thread>
#include <atomic>
#include <mutex>

class Texture;
class File;
class Mesh;
class Resource;
class Timer;

class ModuleResourceManager : public Module
{
public:

	ModuleResourceManager();
	~ModuleResourceManager() = default;

	bool Init() override;

	update_status PreUpdate() override;
	bool CleanUp() override;

	ImportResult Import(const File& file, bool force = false, bool skybox = false);
	void ImportAllFilesInDirectory(const File& file, bool force);
	void CreatePrefab(const std::string& path, GameObject* gameobject_to_save) const;
	uint32_t LoadCubemap(const std::vector<std::string>& faces_paths);

	void CleanResourceCache();

	template<typename T>
	std::shared_ptr<T> Load(const std::string& uid)
	{
		std::shared_ptr<Resource> cache_resource = RetrieveFromCacheIfExist(uid);
		if (cache_resource != nullptr)
		{
			return std::static_pointer_cast<T>(cache_resource);
		}
		ReimportIfNeeded(uid);
		std::shared_ptr<T> resource = Loader::Load<T>(uid);
		if (resource != nullptr)
		{
			
			resource_cache.push_back(resource);
		}
		return resource;
	}

	template<typename T>
	std::shared_ptr<T> Reload(const T * resource) const
	{
		std::string uid = resource->exported_file;
		auto& it = std::find_if(resource_cache.begin(), resource_cache.end(), [resource](const const auto&  loaded_resource) { return loaded_resource.get() == resource; });
		resource_cache.erase(it);
		return Load<T>(uid);
	}
private:
	void StartThread();
	void ReimportIfNeeded(const std::string& uid, bool skybox = false);

	ImportResult InternalImport(const File& file, bool force, bool skybox = false) const;
	std::shared_ptr<Resource> RetrieveFromCacheIfExist(const std::string& uid) const;

public:
	struct ThreadComunication
	{
		mutable std::mutex thread_mutex;
		std::atomic_bool stop_thread = false;
		std::atomic_bool finished_loading = false;
		std::atomic_uint thread_importing_hash = 0;
		std::atomic_uint main_importing_hash = 0;
		std::atomic_uint loaded_items = 0;
		std::atomic_uint total_items = 0;
	} thread_comunication;

	std::unique_ptr<MaterialImporter> material_importer = nullptr;
	std::unique_ptr<TextureImporter> texture_importer = nullptr;
	std::unique_ptr<SceneManager> scene_manager = nullptr;
	std::unique_ptr<ResourceDataBase> resource_DB = nullptr;

private:
	const size_t importer_interval_millis = 60*10*1000;
	float last_imported_time = 0;
	std::thread importing_thread;
	std::unique_ptr<Timer> thread_timer = std::make_unique<Timer>();
	//Importers

	std::unique_ptr<Importer> default_importer = std::make_unique<Importer>();
	std::unique_ptr<ModelImporter> model_importer = nullptr;
	std::unique_ptr<PrefabImporter> prefab_importer = nullptr;
	mutable std::vector<std::shared_ptr<Resource>> resource_cache;

};

#endif // _MODULERESOURCEMANAGER_H_