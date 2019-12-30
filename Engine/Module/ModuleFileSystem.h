#ifndef _ModuleFileSystem_H
#define _ModuleFileSystem_H

#include <Module/Module.h>
#include <string>
#include <vector>
#include <memory>

#include <physfs/physfs.h>

class ModuleFileSystem : public Module
{

public:
	enum class FileType
	{
		MODEL,
		TEXTURE,
		DIRECTORY,
		ARCHIVE,
		UNKNOWN
	};
	struct File {
		File() = default;
		File(const std::string & path, const std::string & name);
		File(const std::string & path);
		std::string filename;
		std::string file_path;
		std::string filename_no_extension;
		ModuleFileSystem::FileType file_type;


		std::vector<std::shared_ptr<File>> childs;
		std::shared_ptr<File> parent;
		bool operator==(const File& compare);
	};

	bool Init() override;
	ModuleFileSystem() = default;
	~ModuleFileSystem() = default;

	char* Load( const char* file_name, size_t & size) const;
	unsigned int Save(const char* file_name, const void* buffer, unsigned int size, bool append = false) const;

	bool Remove(const File & file) const;
	bool Exists(const char* file) const;
	std::string MakeDirectory(const std::string & path, const std::string & directory_name = "new folder");
	bool Copy(const char* source, const char* destination);
	std::shared_ptr<File> GetFileHierarchyFromPath(const std::string & path) const;
	
	FileType GetFileType(const char *file_path, const PHYSFS_FileType & file_type = PHYSFS_FileType::PHYSFS_FILETYPE_OTHER) const;
	void GetAllFilesInPath(const std::string & path, std::vector<std::shared_ptr<File>> & files, bool directories_only = false) const;
	size_t GetNumberOfSubFolders(const std::string & path) const;

	void RefreshFilesHierarchy();
private:
	char *save_path = NULL;
	void GetAllFilesRecursive(std::shared_ptr<File> root) const;
	bool IsValidFileName(const char * file_name) const;
	std::string GetFileExtension(const char *file_path) const;

public:

	std::shared_ptr<File> root_file;
};


#endif // !_ModuleFileSystem_H
