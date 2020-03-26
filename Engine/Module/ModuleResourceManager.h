#ifndef _MODULERESOURCEMANAGER_H_
#define _MODULERESOURCEMANAGER_H_

#include "Main/Application.h"
#include "Module.h"
#include "ModuleFileSystem.h"

#include "ResourceManagement/ResourcesDB/ResourceDataBase.h"

#include <memory>
#include <thread>
#include <atomic>
#include <mutex>

class Mesh;
class Path;
class Resource;
class Texture;
class Timer;

class AnimationImporter;
class MaterialImporter;
class MeshImporter;
class ModelImporter;
class PrefabImporter;
class SceneManager;
class SkeletonImporter;
class TextureImporter;

class MetafileManager;

class ModuleResourceManager : public Module
{
public:

	ModuleResourceManager();
	~ModuleResourceManager() = default;

	bool Init() override;

	update_status PreUpdate() override;
	bool CleanUp() override;

	uint32_t Import(Path& file);
	std::shared_ptr<Resource> Load(uint32_t uuid);
	std::shared_ptr<Resource> Reload(const Resource* resource);

	uint32_t CreateFromData(FileData data, Path& creation_folder_path, const std::string& created_resource_name);

	void CleanInconsistenciesInDirectory(const Path& directory_path);
	void ImportAssetsInDirectory(const Path& directory_path);

	template<typename T>
	uint32_t Create(Path& asset_creation_folder_path, const std::string created_asset_name = "")
	{
		return ResourceManager::Create<T>(asset_creation_folder_path, created_asset_name);
	}

private:
	void StartThread();
	uint32_t InternalImport(Path& file_path) const;

	std::shared_ptr<Resource> RetrieveFromCacheIfExist(uint32_t uuid) const;

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

	//Importers
	std::unique_ptr<AnimationImporter> animation_importer = nullptr;
	std::unique_ptr<MaterialImporter> material_importer = nullptr;
	std::unique_ptr<MeshImporter> mesh_importer = nullptr;
	std::unique_ptr<ModelImporter> model_importer = nullptr;
	std::unique_ptr<PrefabImporter> prefab_importer = nullptr;
	std::unique_ptr<SkeletonImporter> skeleton_importer = nullptr;
	std::unique_ptr<TextureImporter> texture_importer = nullptr;

	std::unique_ptr<MetafileManager> metafile_manager = nullptr;
	std::unique_ptr<SceneManager> scene_manager = nullptr;
	std::unique_ptr<ResourceDataBase> resource_DB = nullptr;

private:
	const size_t importer_interval_millis = 30000;
	float last_imported_time = 0;
	std::thread importing_thread;
	std::unique_ptr<Timer> thread_timer = std::make_unique<Timer>();

	mutable std::vector<std::shared_ptr<Resource>> resource_cache;
};

#endif // _MODULERESOURCEMANAGER_H_