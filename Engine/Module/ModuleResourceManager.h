#ifndef _MODULERESOURCEMANAGER_H_
#define _MODULERESOURCEMANAGER_H_

#include "Main/Application.h"
#include "Module.h"
#include "ModuleFileSystem.h"

#include "ResourceManagement/Resources/Animation.h"
#include "ResourceManagement/Resources/Font.h"
#include "ResourceManagement/Resources/Material.h"
#include "ResourceManagement/Resources/Mesh.h"
#include "ResourceManagement/Resources/Prefab.h"
#include "ResourceManagement/Resources/Skeleton.h"
#include "ResourceManagement/Resources/Skybox.h"
#include "ResourceManagement/Resources/StateMachine.h"
#include "ResourceManagement/Resources/Texture.h"
#include "ResourceManagement/Resources/SoundBank.h"

#include "ResourceManagement/Metafile/MetafileManager.h"
#include "ResourceManagement/ResourcesDB/ResourceDataBase.h"

#include <Brofiler/Brofiler.h>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>

class Path;
class Timer;

class AnimationImporter;
class FontImporter;
class MaterialImporter;
class MeshImporter;
class ModelImporter;
class PrefabImporter;
class SceneImporter;
class SkeletonImporter;
class SkyboxImporter;
class StateMachineImporter;
class SoundImporter;
class TextureImporter;

class SceneManager;

class ModuleResourceManager : public Module
{
public:

	ModuleResourceManager();
	~ModuleResourceManager() = default;

	bool Init() override;

	update_status PreUpdate() override;
	bool CleanUp() override;

	uint32_t Import(Path& file, bool force = false);

	template<typename T>
	uint32_t Create(Path& path, const std::string& resource_name)
	{
		BROFILER_CATEGORY("Create Resource", Profiler::Color::Brown);
		APP_LOG_INFO("Creating Resource %s.", resource_name.c_str())

		FileData created_resource_data = ResourceManagement::Create<T>();

		APP_LOG_SUCCESS("Resource %s created correctly.", resource_name.c_str())

		return CreateFromData(created_resource_data, path, resource_name);
	}

	template<typename T>
	void Save(std::shared_ptr<Resource> modified_resource)
	{
		APP_LOG_INFO("Saving Resource %u.", modified_resource->GetUUID());

		FileData resource_data = ResourceManagement::Binarize<T>(modified_resource.get());
		std::string modified_resource_path = resource_DB->GetEntry(modified_resource->GetUUID())->imported_file_path;
		Path* saved_resource_assets_path = App->filesystem->Save(modified_resource_path, resource_data);

		InternalImport(*saved_resource_assets_path);

		APP_LOG_SUCCESS("Resource %u saved corrrectly.", modified_resource->GetUUID());
	}

	template<typename T>
	std::shared_ptr<T> Load(uint32_t uuid)
	{
		BROFILER_CATEGORY("Load Resource", Profiler::Color::Brown);
		APP_LOG_INFO("Loading Resource %u.", uuid);

		std::shared_ptr<Resource> loaded_resource;
		loaded_resource = RetrieveFromCacheIfExist(uuid);
		if (loaded_resource != nullptr)
		{
			APP_LOG_SUCCESS("Resource %u loaded correctly from cache.", uuid);
			return std::static_pointer_cast<T>(loaded_resource);
		}

		std::string resource_library_file = MetafileManager::GetUUIDExportedFile(uuid);
		if (!App->filesystem->Exists(resource_library_file))
		{
			APP_LOG_ERROR("Error loading Resource %u. File %s doesn't exist", uuid, resource_library_file.c_str());
			return nullptr;
		}

		Path* resource_exported_file_path = App->filesystem->GetPath(resource_library_file);
		FileData exported_file_data = resource_exported_file_path->GetFile()->Load();
		loaded_resource = ResourceManagement::Load<T>(uuid, exported_file_data);

		delete[] exported_file_data.buffer;

		if (loaded_resource != nullptr)
		{
			resource_cache.push_back(loaded_resource);
		}

		APP_LOG_SUCCESS("Resource %u loaded correctly.", uuid);
		return std::static_pointer_cast<T>(loaded_resource);
	}

	void CleanMetafilesInDirectory(const Path& directory_path);
	void ImportAssetsInDirectory(const Path& directory_path, bool force = false);
	void CleanBinariesInDirectory(const Path& directory_path);

	void CleanResourceCache();
	uint32_t CreateFromData(FileData data, Path& creation_folder_path, const std::string& created_resource_name);
	uint32_t CreateFromData(FileData data, const std::string& created_resource_path);

private:

	void StartThread();
	uint32_t InternalImport(Path& file_path, bool force = false) const;
	void RefreshResourceCache();

	std::shared_ptr<Resource> RetrieveFromCacheIfExist(uint32_t uuid) const;

public:
	struct ThreadComunication
	{
		mutable std::mutex thread_mutex;
		std::atomic_bool stop_thread = false;
		std::atomic_bool finished_loading = false;
		std::atomic_uint loaded_items = 0;
		std::atomic_uint total_items = 0;
	} thread_comunication;

	//Importers
	std::unique_ptr<AnimationImporter> animation_importer;
	std::unique_ptr<FontImporter> font_importer;
	std::unique_ptr<MaterialImporter> material_importer;
	std::unique_ptr<MeshImporter> mesh_importer;
	std::unique_ptr<ModelImporter> model_importer;
	std::unique_ptr<PrefabImporter> prefab_importer;
	std::unique_ptr<SceneImporter> scene_importer;
	std::unique_ptr<SkeletonImporter> skeleton_importer;
	std::unique_ptr<SkyboxImporter> skybox_importer;
	std::unique_ptr<StateMachineImporter> state_machine_importer;
	std::unique_ptr<TextureImporter> texture_importer;
	std::unique_ptr<SoundImporter> sound_importer;

	std::unique_ptr<MetafileManager> metafile_manager;
	std::unique_ptr<SceneManager> scene_manager;
	std::unique_ptr<ResourceDataBase> resource_DB;

private:
	const size_t importer_interval_millis = 15 * 1000;
	float last_imported_time = 0;
	std::thread importing_thread;
	std::unique_ptr<Timer> thread_timer = std::make_unique<Timer>();
	
	float cache_time = 0;
	const size_t cache_interval_millis = 15* 1000 ;
	mutable std::vector<std::shared_ptr<Resource>> resource_cache;

	friend class MaterialImporter;
};

#endif // _MODULERESOURCEMANAGER_H_