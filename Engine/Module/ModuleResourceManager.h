#ifndef _MODULERESOURCEMANAGER_H_
#define _MODULERESOURCEMANAGER_H_

#include "Module.h"

#include <memory>
#include <thread>
#include <atomic>

class File;
class ModuleResourceManager : public Module
{
public:

	ModuleResourceManager() = default;

	bool Init() override;

	update_status PreUpdate() override;
	bool CleanUp() override;

	std::pair<bool, std::string> Import(const File& file);
	//Resource Load(const std::string & file);

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


private:
	std::thread importing_thread;
};

#endif // _MODULERESOURCEMANAGER_H_