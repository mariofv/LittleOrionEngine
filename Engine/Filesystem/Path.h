#ifndef _PATH_H_
#define _PATH_H_

#include <vector>
#include <memory>
#include <physfs/physfs.h>

class File;
class ModuleFileSystem;
struct FileData;

class Path 
{
public:
	
	~Path();
	bool operator==(const Path& compare);

	Path* Save(const char* file_name, const FileData& data, bool append = false);
	Path* Save(const char* file_name, const std::string& serialized_data, bool append = false);

	Path* GetParent();
	File* GetFile() const;

	bool IsDirectory() const;
	bool IsMeta() const;
	bool IsImportable() const;

	static std::string GetParentPathString(const std::string& path);
	std::string GetExtension() const;

private:
	Path() = default;
	Path(const std::string& path);
	Path(const std::string& path, const std::string& name);

	void Refresh();

	void CleanFolderPath();
	void GetAllFilesInPath(std::vector<Path*>& path_children, bool directories_only = false);

	void CalculatePathInfo();
	void CalculateFile();
	void CalculateChildren();

public:
	std::string file_path = "";
	std::string file_name = "";
	std::string file_name_no_extension = "";

	std::vector<Path*> children;
	size_t sub_folders = 0;
	size_t total_sub_files_number = 0;

	uint64_t modification_timestamp = 0;

private:
	Path* parent = nullptr;
	File* file = nullptr;

	bool is_directory = false;

	friend ModuleFileSystem;
};

#endif // !_PATH_H_