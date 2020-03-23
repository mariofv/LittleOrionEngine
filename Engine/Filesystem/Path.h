#ifndef _PATH_H_
#define _PATH_H_

#include <vector>
#include <memory>
#include <physfs/physfs.h>

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
	Path(const std::string& path);
	Path(const std::string& path, const std::string& name);
	bool operator==(const Path& compare);

	void Refresh();

private:
	void GetFileInfo();
	void GetChildren();

	FileType GetFileType(const std::string& file_path, const PHYSFS_FileType& file_type = PHYSFS_FileType::PHYSFS_FILETYPE_OTHER) const;
	std::string GetFileExtension(const std::string& file_path) const;

public:
	bool loaded_correctly = true;

	FileType file_type;

	std::string filename;
	std::string file_path;
	std::string filename_no_extension;

	size_t sub_folders = 0;
	size_t total_sub_files_number = 0;
	uint64_t modification_timestamp = 0;

	std::vector<std::shared_ptr<Path>> children;
	Path* parent = nullptr;
};

#endif // !_PATH_H_

