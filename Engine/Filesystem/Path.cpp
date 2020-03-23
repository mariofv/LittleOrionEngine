#include "Path.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"

Path::Path(const std::string & path, const std::string & name)
{
	this->filename = name;
	this->file_path = path + "/" + name;
	this->filename_no_extension = this->filename.substr(0, this->filename.find_last_of("."));
	GetFileInfo();
}

Path::Path(const std::string & path) {

	this->filename = path.substr(path.find_last_of('/') + 1, -1);
	this->file_path = path;
	this->filename_no_extension = this->filename.substr(0, this->filename.find_last_of("."));
	GetFileInfo();
	if (file_type == FileType::DIRECTORY)
	{
		GetChildren();
	}

}
bool Path::operator==(const Path& compare)
{
	return this->filename == compare.filename && this->file_path == compare.file_path && this->file_type == compare.file_type;
};

void Path::GetChildren()
{
	std::vector<std::shared_ptr<Path>> files;
	App->filesystem->GetAllFilesInPath(this->file_path, files);
	for (auto & file : files)
	{
		file->parent = this;
		this->children.push_back(file);
		if (file->file_type == FileType::DIRECTORY)
		{
			file->GetChildren();
			++sub_folders;
			total_sub_files_number += file->total_sub_files_number;
		}
		else {
			++total_sub_files_number;
		}
	}
}

void Path::GetFileInfo()
{
	PHYSFS_Stat file_info;
	if (PHYSFS_stat(this->file_path.c_str(), &file_info) == 0)
	{
		APP_LOG_ERROR("Error getting %s file info: %s", this->file_path.c_str(), PHYSFS_getLastError());
		loaded_correctly = false;
	}
	modification_timestamp = file_info.modtime;
	this->file_type = App->filesystem->GetFileType(filename.c_str(), file_info.filetype);
}

void Path::Refresh()
{
	children.clear();
	GetChildren();
}