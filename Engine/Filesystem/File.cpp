#include "File.h"
#include "Application.h"
#include "Module/ModuleFileSystem.h"
File::File(const std::string & path, const std::string & name) {
	this->filename = name;
	this->file_path = path + "/" + name;
	PHYSFS_Stat file_info;
	if (PHYSFS_stat(this->file_path.c_str(), &file_info) == 0)
	{
		APP_LOG_ERROR("Error getting %s file info: %s", this->file_path.c_str(), PHYSFS_getLastError());
	}

	this->file_type = App->filesystem->GetFileType(filename.c_str(), file_info.filetype);
	this->filename_no_extension = this->filename.substr(0, this->filename.find_last_of("."));
}

File::File(const std::string & path) {

	std::string name = path.substr(path.find_last_of('/') + 1, -1);
	std::string not_name_path = path.substr(0, path.find_last_of('/'));
	*this = File(not_name_path, name);
}
bool File::operator==(const File& compare)
{
	return this->filename == compare.filename && this->file_path == compare.file_path && this->file_type == compare.file_type;
};
