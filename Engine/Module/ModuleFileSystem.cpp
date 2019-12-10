#include "ModuleFileSystem.h"


unsigned int ModuleFileSystem::Load(const char* path, const char* file, char** buffer) const
{
	return 0;
}
unsigned int ModuleFileSystem::Save(const char* file, const void* buffer, unsigned int size, bool append) const
{
	return 0;
}
bool ModuleFileSystem::Remove(const char* file)
{
	return true;
}
bool ModuleFileSystem::Exists(const char* file) const
{
	return true;
}
bool ModuleFileSystem::MakeDirectory(const std::string & directory, const std::string & directory_name)
{
	std::string new_directory = directory + "\\" + directory_name;
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

ModuleFileSystem::FileType ModuleFileSystem::GetFileType(const char *file_path) const
{
	std::string file_extension = GetFileExtension(file_path);

	if (
		file_extension == "png"
		|| file_extension == "PNG"
		|| file_extension == "dds"
		|| file_extension == "DDS"
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

	return ModuleFileSystem::FileType::UNKNOWN;
}


std::string ModuleFileSystem::GetFileExtension(const char *file_path) const
{
	std::string file_path_string = std::string(file_path);
	std::size_t found = file_path_string.find_last_of(".");

	std::string file_extension = file_path_string.substr(found + 1, file_path_string.length());

	return file_extension;
}

void ModuleFileSystem::GetAllFilesInPath(const std::string & path, std::vector<File> & files)
{

}
void ModuleFileSystem::GetAllFilesInPathRecursive(const std::string & path, std::vector<File> & files)
{

}