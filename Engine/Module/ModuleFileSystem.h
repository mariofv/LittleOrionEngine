#ifndef _ModuleFileSystem_H
#define _ModuleFileSystem_H

#include <Module/Module.h>
#include <string.h>
#include <vector>
#include <memory>
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
		std::string filename;
		std::string file_path;
		ModuleFileSystem::FileType file_type;
		std::vector<std::shared_ptr<File>> childs;
		std::shared_ptr<File> parent;
	};

	ModuleFileSystem() = default;
	~ModuleFileSystem() = default;

	unsigned int Load(const char* path, const char* file, char** buffer) const;
	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false) const;

	bool Remove(const char* file);
	bool Exists(const char* file) const;
	bool MakeDirectory(const std::string & path, const std::string & directory_name = "new folder");
	bool IsDirectory(const char* file) const;
	bool Copy(const char* source, const char* destination);


	FileType GetFileType(const char *file_path, const DWORD & file_attributes = FILE_ATTRIBUTE_ARCHIVE) const;
	void GetAllFilesInPath(const std::string & path, std::vector<std::shared_ptr<File>> & files) const;
	void GetAllFilesInPathRecursive(const std::string & path, std::vector<std::shared_ptr<File>> & files) const;
	size_t GetNumberOfSubFolders(const std::string & path) const;
private:
	std::string GetFileExtension(const char *file_path) const;

};

#endif // !_ModuleFileSystem_H
