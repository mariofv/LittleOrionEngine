#ifndef _MODULEFILESYSTEM_H_
#define _MODULEFILESYSTEM_H_

#include "Module/Module.h"
#include "Filesystem/Path.h"

#include <memory>
#include <physfs/physfs.h>
#include <string>
#include <unordered_map>
#include <vector>


class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem() = default;
	~ModuleFileSystem();

	bool Init() override;
	bool CleanUp() override;

	void AddPath(Path* path);
	Path* GetPath(const std::string& path);
	Path* GetRootPath() const;

	Path* Save(const std::string& save_path, FileData data_to_save);
	Path* Save(const std::string& save_path, const std::string& serialized_data);

	bool Exists(const std::string& path) const;

	bool Remove(Path* path);
	bool Remove(const std::string& path);

	Path* Copy(const std::string& source_path, const std::string& destination_path, const std::string& copied_file_name = std::string());
	Path* MakeDirectory(const std::string& new_directory_full_path);
	
	bool MountDirectory(const std::string& directory) const;
	bool CreateMountedDir(const std::string& directory);

private:
	void RefreshPathMap(); // TODO: Not allocate all at the begining, do it by demand!

public:
	Path* assets_folder_path = nullptr;
	Path* library_folder_path = nullptr;
	Path* resources_folder_path = nullptr;

private:
	Path* root_path = nullptr;
	std::unordered_map<std::string, Path*> paths;
};


#endif // !_MODULEFILESYSTEM_H_
