#ifndef _MODULERESOURCEMANAGER_H_
#define _MODULERESOURCEMANAGER_H_

#include "Module.h"

#include <memory>
#include <thread>
#include <atomic>

class Texture;
class File;
class Mesh;
class Importer;
class Timer;
class TextureImporter;
class ModelImporter;
class ModuleResourceManager : public Module
{
public:

	ModuleResourceManager() = default;

	bool Init() override;

	update_status PreUpdate() override;
	bool CleanUp() override;

	std::pair<bool, std::string> Import(const File& file);
	std::shared_ptr<Texture> LoadTexture(const std::string& file_path) const;
	std::shared_ptr<Mesh> LoadModel(const std::string& file_path) const;
	/*template<class T>
	std::shared_ptr<T> Load(const std::string& uid)
	{
		return Loader::LoadResource(uid);
	}*/

private:
	std::pair<bool, std::string> InternalImport(const File& file);
	void ImportAllFileHierarchy(const File& file);
	void StartThread();

public:
	struct ThreadComunication
	{
		std::atomic_bool stop_thread = false;
		std::atomic_bool finished_loading = false;
		std::atomic_uint importing_hash = 0;
		std::atomic_uint loaded_items = 0;
		std::atomic_uint total_items = 0;
	} thread_comunication;

	std::unique_ptr<TextureImporter> texture_importer = nullptr;
	std::unique_ptr<ModelImporter> model_importer = nullptr;
private:
	const size_t importer_interval_millis = 30000;
	float last_imported_time = 0;
	std::thread importing_thread;
	std::unique_ptr<Timer> thread_timer = std::make_unique<Timer>();
	//Importers

	std::unique_ptr<Importer> default_importer = std::make_unique<Importer>();

};

#endif // _MODULERESOURCEMANAGER_H_