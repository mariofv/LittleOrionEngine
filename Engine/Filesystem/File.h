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
struct File {
	File() = default;
	File(const std::string & path, const std::string & name);
	File(const std::string & path);
	std::string filename;
	std::string file_path;
	std::string filename_no_extension;
	FileType file_type;


	std::vector<std::shared_ptr<File>> children;
	File* parent;
	bool operator==(const File& compare);
};

#endif // !_PATH_H_

