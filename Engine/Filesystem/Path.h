#ifndef _PATH_H_
#define _PATH_H_

#include <vector>
#include <memory>

enum class FileType
{
	MODEL,
	PREFAB, 
	TEXTURE,
	MATERIAL,
	MESH,
	DIRECTORY,
	ARCHIVE,
	UNKNOWN
};

class Path 
{
public:
	Path() = default;
	Path(const std::string & path, const std::string & name);
	Path(const std::string & path);
	std::string filename;
	std::string file_path;
	std::string filename_no_extension;
	FileType file_type;
	size_t sub_folders = 0;
	size_t total_sub_files_number = 0;
	uint64_t modification_timestamp = 0;

	std::vector<std::shared_ptr<Path>> children;
	Path* parent = nullptr;
	bool operator==(const Path& compare);

	void Refresh();
public:
	bool loaded_correctly = true;

private:
	void GetFileInfo();
	void GetChildren();

};

#endif // !_PATH_H_

