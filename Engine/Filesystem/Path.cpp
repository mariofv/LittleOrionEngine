#include "Path.h".

#include "File.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"

#include <SDL/SDL.h>

Path::Path(const std::string& path) 
{
	assert(App->filesystem->Exists(path));
	CleanFolderPath();
	file_path = path;
	
	Refresh();
}

Path::Path(const std::string& path, const std::string& name)
{
	assert(App->filesystem->Exists(path));
	CleanFolderPath();
	file_path = path + "/" + name;

	Refresh();
}

Path::~Path()
{
	delete file;
}

bool Path::operator==(const Path& compare)
{
	return file_path == compare.file_path;
}

void Path::Refresh()
{
	CalculatePathInfo();
	if (is_directory)
	{
		CalculateChildren();
	}
}

void Path::RemoveChild(Path* child_to_remove)
{
	auto it = std::find(children.begin(), children.end(), child_to_remove);
	assert(it != children.end());
	children.erase(it);
}

Path* Path::Save(const char* file_name, const FileData& data, bool append)
{
	assert(is_directory);
	if (data.size == 0)
	{
		return nullptr;
	}

	std::string saved_file_path_string = file_path + "/" + file_name;
	bool already_exists = App->filesystem->Exists(saved_file_path_string);

	PHYSFS_File * file;
	if (append)
	{
		file = PHYSFS_openAppend(saved_file_path_string.c_str());
	}
	else
	{
		file = PHYSFS_openWrite(saved_file_path_string.c_str());
	}

	if (file == NULL)
	{
		APP_LOG_ERROR("Error: Unable to open file! PhysFS Error: %s\n", PHYSFS_getLastError());
		return nullptr;
	}


	PHYSFS_writeBytes(file, data.buffer, data.size);
	APP_LOG_INFO("File %s saved!\n", saved_file_path_string.c_str());
	PHYSFS_close(file);

	Path* saved_file_path = nullptr;
	if (!already_exists)
	{
		saved_file_path = App->filesystem->AddPath(saved_file_path_string);
		children.push_back(saved_file_path);
		saved_file_path->parent = this;
	}
	else
	{
		saved_file_path = App->filesystem->GetPath(saved_file_path_string);
	}

	delete[] data.buffer;

	return saved_file_path;
}

Path* Path::Save(const char* file_name, const std::string& serialized_data, bool append)
{
	char* data_bytes = new char[serialized_data.size() + 1];
	memcpy(data_bytes, serialized_data.c_str(), serialized_data.size() + 1);

	FileData data{ data_bytes, serialized_data.size() + 1 };
	return Save(file_name, data);
}

Path* Path::GetParent() const
{
	return parent;
}

std::string Path::GetParentPathString(const std::string& path)
{
	std::size_t found = path.find_last_of("/");
	if (found == std::string::npos || found == 0) {
		return "";
	}
	return path.substr(0, found);
}


File* Path::GetFile() const
{
	if (file == nullptr)
	{
		APP_LOG_ERROR("Path %s doesn't contain a file!", file_path.c_str())
	}

	return file;
}

void Path::CalculatePathInfo()
{
	PHYSFS_Stat path_info;
	if (PHYSFS_stat(file_path.c_str(), &path_info) == 0)
	{
		APP_LOG_ERROR("Error getting %s path info: %s", file_path.c_str(), PHYSFS_getLastError());
	}

	std::string file_extension = GetExtension(file_path);
	is_directory = (PHYSFS_FileType::PHYSFS_FILETYPE_DIRECTORY == path_info.filetype);
	CalculateFile();
}

void Path::CalculateFile()
{
	if (is_directory)
	{
		file = nullptr;
	}
	else
	{
		file = new File(this);
	}
}

void Path::CalculateChildren()
{
	std::vector<Path*> path_children;
	GetAllFilesInPath(path_children);

	for (auto & path_child : path_children)
	{
		path_child->parent = this;
		this->children.push_back(path_child);
		if (path_child->IsDirectory())
		{
			++sub_folders;
			total_sub_files_number += path_child->total_sub_files_number;
		}
		else
		{
			++total_sub_files_number;
		}
	}
}

uint32_t Path::GetModificationTimestamp() const
{
	PHYSFS_Stat path_info;
	if (PHYSFS_stat(file_path.c_str(), &path_info) == 0)
	{
		APP_LOG_ERROR("Error getting %s path info: %s", file_path.c_str(), PHYSFS_getLastError());
	}

	return path_info.modtime;
}

void Path::GetAllFilesInPath(std::vector<Path*>& path_children)
{
	char **files_array = PHYSFS_enumerateFiles(file_path.c_str());
	if (*files_array == NULL)
	{
		APP_LOG_INFO("Error reading directory %s: %s", file_path.c_str(), PHYSFS_getLastError());
		return;
	}

	char **filename;
	for (filename = files_array; *filename != NULL; filename++)
	{
		Path* child_path = App->filesystem->AddPath(file_path + '/' +  *filename);
		path_children.push_back(child_path);
	}
	PHYSFS_freeList(files_array);
}

bool Path::IsDirectory() const
{
	return is_directory;
}

bool Path::IsMeta() const
{
	return !is_directory && file->GetFileType() == FileType::META;
}

bool Path::IsImportable() const
{
	FileType file_type = file->GetFileType();
	return
		file_type == FileType::ANIMATION
		|| file_type == FileType::MATERIAL
		|| file_type == FileType::MESH
		|| file_type == FileType::MODEL
		|| file_type == FileType::PREFAB
		|| file_type == FileType::SCENE
		|| file_type == FileType::SKELETON
		|| file_type == FileType::SKYBOX
		|| file_type == FileType::STATE_MACHINE
		|| file_type == FileType::TEXTURE;
}

bool Path::IsBinary() const
{
	return !is_directory && GetExtension() == "";
}

std::string Path::GetFullPath() const
{
	return file_path;
}

std::string Path::GetFilename() const
{
	return file_path.substr(file_path.find_last_of('/') + 1, -1);
}

std::string Path::GetFilenameWithoutExtension() const
{
	std::string file_name = GetFilename();
	return file_name.substr(0, file_name.find_last_of("."));
}

std::string Path::GetFullPathWithoutExtension() const
{
	return file_path.substr(0, file_path.find_last_of("."));
}

std::string Path::GetExtension() const
{
	std::size_t found = file_path.find_last_of(".");
	if (found == std::string::npos || found == 0) {
		return "";
	}
	std::string file_extension = file_path.substr(found + 1, file_path.length());

	return file_extension;
}

std::string Path::GetExtension(const std::string& path)
{
	std::size_t found = path.find_last_of(".");
	if (found == std::string::npos || found == 0) {
		return "";
	}
	std::string file_extension = path.substr(found + 1, path.length());

	return file_extension;
}

void Path::CleanFolderPath()
{
	if (!file_path.empty() && file_path.back() == '/')
	{
		file_path.pop_back();
	}
}