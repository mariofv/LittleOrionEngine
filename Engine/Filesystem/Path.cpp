#include "Path.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"

#include <algorithm>
#include <cctype>

Path::Path(const std::string& path) {

	this->filename = path.substr(path.find_last_of('/') + 1, -1);
	this->file_path = path;
	this->filename_no_extension = this->filename.substr(0, this->filename.find_last_of("."));
	GetFileInfo();
	if (file_type == FileType::DIRECTORY)
	{
		GetChildren();
	}

}

Path::Path(const std::string& path, const std::string& name)
{
	this->filename = name;
	this->file_path = path + "/" + name;
	this->filename_no_extension = this->filename.substr(0, this->filename.find_last_of("."));
	GetFileInfo();
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
	file_type = GetFileType(filename.c_str(), file_info.filetype);
}

void Path::Refresh()
{
	children.clear();
	GetChildren();
}

FileType Path::GetFileType(const std::string& file_path, const PHYSFS_FileType& file_type) const
{
	std::string file_extension = GetFileExtension(file_path);
	std::transform(file_extension.begin(), file_extension.end(), file_extension.begin(),
		[](unsigned char letter) { return std::tolower(letter); });

	if ((PHYSFS_FileType::PHYSFS_FILETYPE_DIRECTORY == file_type) || (file_extension == "/"))
	{
		return FileType::DIRECTORY;
	}

	if (
		file_extension == "png"
		|| file_extension == "tif"
		|| file_extension == "dds"
		|| file_extension == "tga"
		|| file_extension == "jpg"
		|| file_extension == "jfif"
		)
	{
		return FileType::TEXTURE;
	}
	if (file_extension == "fbx")
	{
		return FileType::MODEL;
	}
	if (file_extension == "prefab")
	{
		return FileType::PREFAB;
	}
	if (file_extension == "ol" || file_extension == "mesh")
	{
		return FileType::MESH;
	}
	if (file_extension == "matol")
	{
		return FileType::MATERIAL;
	}
	if (file_extension == "" && PHYSFS_FileType::PHYSFS_FILETYPE_OTHER == file_type)
	{
		return FileType::ARCHIVE;
	}
	return FileType::UNKNOWN;
}


std::string Path::GetFileExtension(const std::string& file_path) const
{
	std::string file_path_string = std::string(file_path);
	if (!file_path_string.empty() && file_path_string.back() == '/')
	{
		return std::string("/");
	}

	std::size_t found = file_path_string.find_last_of(".");
	if (found == std::string::npos || found == 0) {
		return "";
	}
	std::string file_extension = file_path_string.substr(found + 1, file_path_string.length());

	return file_extension;
}
