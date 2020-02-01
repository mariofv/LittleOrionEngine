#ifndef _PATH_H_
#define _PATH_H_

#include <vector>
#include <memory>
enum class FileType
{
	MODEL,
	TEXTURE,
	DIRECTORY,
	ARCHIVE,
	UNKNOWN
};
class File {
public:
	File() = default;
	File(const std::string & path, const std::string & name);
	File(const std::string & path);
	std::string filename;
	std::string file_path;
	std::string filename_no_extension;
	FileType file_type;
	size_t sub_folders = 0;
	size_t total_sub_files_number = 0;
	uint64_t modification_timestamp = 0;

	std::vector<std::shared_ptr<File>> children;
	File* parent;
	bool operator==(const File& compare);
private:
	void GetFileInfo();
	void GetChildren();

};

#endif // !_PATH_H_

