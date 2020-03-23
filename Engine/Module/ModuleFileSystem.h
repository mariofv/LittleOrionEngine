#ifndef _MODULEFILESYSTEM_H_
#define _MODULEFILESYSTEM_H_

#include "Module/Module.h"
#include "Filesystem/Path.h"

#include <memory>
#include <physfs/physfs.h>
#include <string>
#include <vector>


class ModuleFileSystem : public Module
{
public:

	bool Init() override;
	bool CleanUp() override;
	ModuleFileSystem() = default;
	~ModuleFileSystem();

	bool Save(const char* file_name, const void* buffer, unsigned int size, bool append = false) const;
	char* Load(const char* file_name, size_t& size) const;

	bool Exists(const std::string& path) const;

	bool Remove(const Path* path) const;
	bool Remove(const std::string& path) const;

	bool Copy(const std::string& source, const std::string& destination);
	Path MakeDirectory(const std::string& new_directory_full_path) const;
	
	void GetAllFilesInPath(const std::string & path, std::vector<std::shared_ptr<Path>> & files, bool directories_only = false) const;

	bool CreateMountedDir(const char * directory) const;
	void RefreshFilesHierarchy();

public:
	std::shared_ptr<Path> assets_file;
};


#endif // !_MODULEFILESYSTEM_H_
