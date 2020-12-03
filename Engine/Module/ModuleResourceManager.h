#ifndef _MODULERESOURCEMANAGER_H_
#define _MODULERESOURCEMANAGER_H_

#include "Log/EngineLog.h"

#include "Main/Application.h"
#include "Module.h"
#include "ModuleFileSystem.h"
#include "Module/ModuleTime.h"
#include "Helper/ThreadSafeQueue.h"
#include "Helper/Timer.h"

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
#include "ResourceManagement/Resources/Video.h"

#include "ResourceManagement/Metafile/MetafileManager.h"
#include "ResourceManagement/ResourcesDB/ResourceDataBase.h"

#include <atomic>
#include <Brofiler/Brofiler.h>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>

#define MULTITHREADING 1

class Path;
class Timer;

class Importer;
class AnimationImporter;
class MaterialImporter;
class MeshImporter;
class ModelImporter;
class PrefabImporter;
class SkeletonImporter;
class StateMachineImporter;
class SoundImporter;
class TextureImporter;

class SceneManager;

struct LoadingJob
{
	uint32_t uuid = 0;
	bool already_in_cache = false;
	Component* component_to_load = nullptr;

	//For material type enum
	ResourceType resource_type = ResourceType::TEXTURE;
	unsigned texture_type = 0;
};


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
		RESOURCES_LOG_INFO("Creating Resource %s.", resource_name.c_str())

		FileData created_resource_data = ResourceManagement::Create<T>();

		RESOURCES_LOG_INFO("Resource %s created correctly.", resource_name.c_str())

		return CreateFromData(created_resource_data, path, resource_name);
	}

	template<typename T>
	void Save(std::shared_ptr<Resource> modified_resource)
	{
		RESOURCES_LOG_INFO("Saving Resource %u.", modified_resource->GetUUID());

		FileData resource_data = ResourceManagement::Binarize<T>(modified_resource.get());
		std::string modified_resource_path = resource_DB->GetEntry(modified_resource->GetUUID())->imported_file_path;
		Path* saved_resource_assets_path = App->filesystem->Save(modified_resource_path, resource_data);

		InternalImport(*saved_resource_assets_path);

		RESOURCES_LOG_INFO("Resource %u saved corrrectly.", modified_resource->GetUUID());
	}

	template<typename T>
	std::shared_ptr<T> Load(uint32_t uuid)
	{
		BROFILER_CATEGORY("Load Resource", Profiler::Color::Brown);
		RESOURCES_LOG_INFO("Loading Resource %u.", uuid);

		std::shared_ptr<Resource> loaded_resource;
		loaded_resource = RetrieveFromCacheIfExist(uuid);
		if (loaded_resource != nullptr)
		{
			RESOURCES_LOG_INFO("Resource %u loaded correctly from cache.", uuid);
			return std::static_pointer_cast<T>(loaded_resource);
		}

		//HERE WE CHECK IF T IS TEXTURE AND IF SO WE ADD FILEDATA TO THE QUEUE
		//WE NEED TO CHECK HOW I AM GONNA SOLVE THE LOADED_RESOURCE NULLPTR WHILE NOT BEING CREATED
		
		if(MULTITHREADING && App->time->isGameRunning() && (std::is_same<T, Texture>::value || std::is_same<T, Mesh>::value || std::is_same<T, Font>::value) && !loading_thread_communication.normal_loading_flag && loading_thread_communication.load_scene_asyncronously)
		{
			loaded_resource = nullptr;

			LoadingJob load_job;

			load_job.component_to_load = loading_thread_communication.current_component_loading;
			load_job.uuid = uuid;
			load_job.resource_type = loading_thread_communication.current_type;
			load_job.texture_type = loading_thread_communication.texture_type;
			loading_resources_queue.Push(load_job);
			++loading_thread_communication.total_number_of_resources_to_load;
		}
		else
		{
			std::string resource_library_file = MetafileManager::GetUUIDExportedFile(uuid);
			if (!App->filesystem->Exists(resource_library_file))
			{
				RESOURCES_LOG_ERROR("Error loading Resource %u. File %s doesn't exist", uuid, resource_library_file.c_str());
				return nullptr;
			}

			Path* resource_exported_file_path = App->filesystem->GetPath(resource_library_file);
			FileData exported_file_data = resource_exported_file_path->GetFile()->Load();

			loaded_resource = ResourceManagement::Load<T>(uuid, exported_file_data);
			//IMPORTANT IF TEXTURE THIS MUST BE DELETED
			delete[] exported_file_data.buffer;
		}
		

		if (loaded_resource != nullptr)
		{
			resource_cache.push_back(loaded_resource);
		}

		RESOURCES_LOG_INFO("Resource %u loaded correctly.", uuid);
		return std::static_pointer_cast<T>(loaded_resource);
	}

	void CleanMetafilesInDirectory(const Path& directory_path);
	void ImportAssetsInDirectory(const Path& directory_path, bool force = false);
	void CleanBinariesInDirectory(const Path& directory_path);
	
	void AddResourceToCache(std::shared_ptr<Resource> resource);
	void CleanResourceCache();
	bool CleanResourceFromCache(uint32_t uuid);
	uint32_t CreateFromData(FileData data, Path& creation_folder_path, const std::string& created_resource_name);
	uint32_t CreateFromData(FileData data, const std::string& created_resource_path);

	//Multithread loader
	void LoaderThread();
	void RemoveUUIDFromCache(uint32_t uuid);
	std::shared_ptr<Resource> RetrieveFromCacheIfExist(uint32_t uuid) const;

	bool RetrieveFileDataByUUID(uint32_t uuid, FileData& filedata) const;

private:

	void StartThread();
	uint32_t InternalImport(Path& file_path, bool force = false) const;
	void RefreshResourceCache();


public:
	bool first_import_completed = false;
	//Multithreading
	struct ThreadComunication
	{
		mutable std::mutex thread_mutex;
		std::atomic_bool stop_thread = false;
		std::atomic_bool finished_loading = false;
		std::atomic_uint loaded_items = 0;
		std::atomic_uint total_items = 0;
	} thread_comunication;

	float time_loading_meshes = 0.f;

	std::vector<std::shared_ptr<Prefab>> prefabs_to_reassign;

	ThreadSafeQueue<LoadingJob> loading_resources_queue;
	ThreadSafeQueue<LoadingJob> processing_resources_queue;

	struct LoadingTexturesThreadCommunication
	{
		std::atomic_bool loading = false;
		std::atomic<int> total_number_of_resources_to_load = 0;
		std::atomic<int> current_number_of_resources_loaded = 0;

		//Asyncronous loading variables
		int max_threads = 1;
		std::atomic<bool> loading_threads_active = true;
		std::vector<std::thread> loader_threads;
		bool normal_loading_flag = false;
		bool load_scene_asyncronously = true;
		bool restore_time_scale = false;
		//We change this value to the component that calls Load for textures
		Component* current_component_loading = nullptr;
		ResourceType current_type = ResourceType::TEXTURE;
		//Texture Type 
		unsigned texture_type = 0;

		//Debugging variables
		int number_of_textures_loaded = 0;

	}loading_thread_communication;

	//Importers
	std::unique_ptr<AnimationImporter> animation_importer;
	std::unique_ptr<MaterialImporter> material_importer;
	std::unique_ptr<MeshImporter> mesh_importer;
	std::unique_ptr<ModelImporter> model_importer;
	std::unique_ptr<PrefabImporter> prefab_importer;
	std::unique_ptr<SkeletonImporter> skeleton_importer;
	std::unique_ptr<StateMachineImporter> state_machine_importer;
	std::unique_ptr<TextureImporter> texture_importer;
	std::unique_ptr<SoundImporter> sound_importer;
	std::unique_ptr<Importer> generic_importer;

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

	Timer timer = Timer();

	friend class MaterialImporter;
};

#endif // _MODULERESOURCEMANAGER_H_