#include "ModuleFileSystem.h"
#include "Application.h"
#include <algorithm>

unsigned int ModuleFileSystem::Load(const char* path, const char* file, char** buffer) const
{
	return 0;
}
unsigned int ModuleFileSystem::Save(const char* file, const void* buffer, unsigned int size, bool append) const
{
	return 0;
}
bool ModuleFileSystem::Remove(const File & file) const
{
	if (file.file_type == FileType::DIRECTORY) {
		return RemoveDirectoryA(file.file_path.c_str());
	}
	return DeleteFileA(file.file_path.c_str());
}
bool ModuleFileSystem::Exists(const char* file) const
{
	return true;
}
bool ModuleFileSystem::MakeDirectory(const std::string & path, const std::string & directory_name)
{
	std::vector<std::shared_ptr<File>> files;
	GetAllFilesInPath(path + "//*", files);

	std::vector<std::shared_ptr<File>> same_name_folders;
	same_name_folders.reserve(files.size());

	std::copy_if(files.begin(), files.end(), std::back_inserter(same_name_folders), [directory_name](const std::shared_ptr<File> file) {
		size_t last_bracket = file->filename.find_last_of("(");
		return file->file_type == FileType::DIRECTORY && file->filename.substr(0,last_bracket - 1) == directory_name;
	});
	std::string new_directory;
	do {
		new_directory = path + "//" + directory_name + " (" + std::to_string(same_name_folders.size()) + ")";
		same_name_folders.clear();
		std::copy_if(files.begin(), files.end(), std::back_inserter(same_name_folders), [directory_name](const std::shared_ptr<File> file) {
			return file->file_type == FileType::DIRECTORY && file->filename == directory_name;
		});
	} while (same_name_folders.size() != 0);

	CreateDirectory(new_directory.c_str(), NULL);
	return true;
}
bool ModuleFileSystem::IsDirectory(const char* file) const 
{
	return true;
}
bool ModuleFileSystem::Copy(const char* source, const char* destination)
{
	return true;
}

ModuleFileSystem::FileType ModuleFileSystem::GetFileType(const char *file_path, const DWORD & file_attributes) const
{
	std::string file_extension = GetFileExtension(file_path);

	if (
		file_extension == "png"
		|| file_extension == "PNG"
		|| file_extension == "dds"
		|| file_extension == "DDS"
		|| file_extension == "tga"
		|| file_extension == "JPG"
		)
	{
		return ModuleFileSystem::FileType::TEXTURE;
	}
	if (
		file_extension == "fbx"
		|| file_extension == "FBX"
		)
	{
		return ModuleFileSystem::FileType::MODEL;
	}
	if (file_extension == "" && file_attributes & FILE_ATTRIBUTE_DIRECTORY) 
	{
		return ModuleFileSystem::FileType::DIRECTORY;
	}
	if (file_extension == "" && file_attributes & FILE_ATTRIBUTE_ARCHIVE)
	{
		return ModuleFileSystem::FileType::ARCHIVE;
	}
	return ModuleFileSystem::FileType::UNKNOWN;
}


std::string ModuleFileSystem::GetFileExtension(const char *file_path) const
{
	std::string file_path_string = std::string(file_path);
	std::size_t found = file_path_string.find_last_of(".");
	if (found == std::string::npos) {
		return "";
	}
	std::string file_extension = file_path_string.substr(found + 1, file_path_string.length());

	return file_extension;
}

void ModuleFileSystem::GetAllFilesInPath(const std::string & path, std::vector<std::shared_ptr<File>> & files, bool directories_only) const
{
	std::string path_all = path + "//*";

	WIN32_FIND_DATA find_file_data;
	HANDLE handle_find = FindFirstFile(path_all.c_str(), &find_file_data);
	if (handle_find == INVALID_HANDLE_VALUE) {
		return;
	}
	do {
		bool is_directory = find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
		if (IsValidFileName(find_file_data.cFileName) && ((directories_only && is_directory) || !directories_only))
		{
			std::shared_ptr<File> new_file = std::make_shared<File>(find_file_data,path);
			files.push_back(new_file);
		}
	} while (FindNextFile(handle_find, &find_file_data) != 0);

	FindClose(handle_find);
}

std::shared_ptr<ModuleFileSystem::File> ModuleFileSystem::GetFileHierarchyFromPath(const std::string & path) const
{
	std::shared_ptr<File> new_file = std::make_shared<File>();
	new_file->filename = path;
	GetAllFilesRecursive(new_file);
	return new_file;
}
void ModuleFileSystem::GetAllFilesRecursive(std::shared_ptr<File> root) const
{
	std::vector<std::shared_ptr<File>> files;
	GetAllFilesInPath(root->file_path, files);
	for (auto & file : files )
	{
		file->parent = root;
		root->childs.push_back(file);
		GetAllFilesRecursive(file);

	}
}

size_t ModuleFileSystem::GetNumberOfSubFolders(const std::string & path) const
{
	std::string path_all = path + "//*";
	WIN32_FIND_DATA find_file_data;
	HANDLE handle_find = FindFirstFile(path_all.c_str(), &find_file_data);

	if (handle_find == INVALID_HANDLE_VALUE) {
		return 0;
	}
	size_t subFiles = 0;
	do {
		if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && IsValidFileName(find_file_data.cFileName))
		{
			++subFiles;
		}

	} while (FindNextFile(handle_find, &find_file_data) != 0);

	FindClose(handle_find);
	return subFiles;
}

bool ModuleFileSystem::IsValidFileName(const char * file_name) const
{
	return std::strcmp(file_name, ".") && std::strcmp(file_name, "..");
}
bool ModuleFileSystem::File::operator==(const ModuleFileSystem::File& compare)
{
	return this->filename == compare.filename && this->file_path == compare.file_path && this->file_type == compare.file_type;
};
ModuleFileSystem::File::File(const WIN32_FIND_DATA & windows_file_data, const std::string & path) {
	this->filename = windows_file_data.cFileName;
	this->file_path = path + "//" + windows_file_data.cFileName;
	this->file_type = App->filesystem->GetFileType(filename.c_str(), windows_file_data.dwFileAttributes);
}
