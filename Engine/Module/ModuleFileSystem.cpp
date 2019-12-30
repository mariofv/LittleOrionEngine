#include "ModuleFileSystem.h"
#include "Application.h"
#include <SDL/SDL.h>

#include <algorithm>
#include <cctype>

bool ModuleFileSystem::Init() {
	save_path = SDL_GetPrefPath("UPC",TITLE);
	if (save_path == NULL)
	{
		return false;
	}
	if (PHYSFS_setWriteDir(".") == 0)
	{
		APP_LOG_ERROR("Error creating writing directory: %s", PHYSFS_getLastError());
		return false;
	}
	MakeDirectory("Assets");
	MakeDirectory("Assets/Scenes");
	MakeDirectory("Library/Materials");
	if (PHYSFS_mount("Assets", "Assets", 1) == 0)
	{
		APP_LOG_ERROR("Error mounting directory: %s", PHYSFS_getLastError());
		return false;
	}
	if (PHYSFS_mount("Library", "Library", 1) == 0)
	{
		APP_LOG_ERROR("Error mounting directory: %s", PHYSFS_getLastError());
		return false;
	}
	RefreshFilesHierarchy();
	return true;
}
char* ModuleFileSystem::Load(const char* file_path, size_t & size) const
{
	SDL_RWops *rw = SDL_RWFromFile(file_path, "rb");
	if (rw == NULL) return NULL;

	Sint64 res_size = SDL_RWsize(rw);
	char* res = (char*)malloc(res_size + 1);

	Sint64 nb_read_total = 0, nb_read = 1;
	char* buf = res;
	while (nb_read_total < res_size && nb_read != 0) {
		nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
		nb_read_total += nb_read;
		buf += nb_read;
	}
	SDL_RWclose(rw);
	if (nb_read_total != res_size) {
		free(res);
		return NULL;
	}

	res[nb_read_total] = '\0';
	if (size != NULL)
		size = nb_read_total;
	return res;

}
unsigned int ModuleFileSystem::Save(const char* file_path, const void* buffer, unsigned int size, bool append) const
{
	SDL_RWops* file;
	if (append) 
	{
		file = SDL_RWFromFile(file_path, "a+b");
	}
	else 
	{
		file = SDL_RWFromFile(file_path, "w+b");
	}

	if (file != NULL)
	{
		APP_LOG_INFO("File %s saved!\n", file_path);

	SDL_RWwrite(file, buffer, size, 1);

	}
	else
	{
		APP_LOG_ERROR("Error: Unable to open file! SDL Error: %s\n", SDL_GetError());
		return 1;
	}
	SDL_RWclose(file);
	return 0;
}
bool ModuleFileSystem::Remove(const File & file) const
{
	return PHYSFS_delete(file.file_path.c_str()) != 0;
}
bool ModuleFileSystem::Exists(const char* file_path) const
{
	SDL_RWops* file = SDL_RWFromFile(file_path, "r");
	bool exists = file != NULL;
	SDL_RWclose(file);
	return exists;
}

std::string ModuleFileSystem::MakeDirectory(const std::string & new_directory_full_path) const
{
	if (PHYSFS_mkdir(new_directory_full_path.c_str()) == 0)
	{
		APP_LOG_ERROR("Error creating directory %s : %s", new_directory_full_path.c_str(), PHYSFS_getLastError());
		return "";
	}
	return new_directory_full_path;
}
bool ModuleFileSystem::Copy(const char* source, const char* destination) const
{
	size_t file_size;
	char * buffer = Load(source,file_size);
	return Save(destination, buffer, file_size,false);
}

ModuleFileSystem::FileType ModuleFileSystem::GetFileType(const char *file_path, const PHYSFS_FileType & file_type) const
{
	std::string file_extension = GetFileExtension(file_path);
	std::transform(file_extension.begin(), file_extension.end(), file_extension.begin(),
		[](unsigned char letter) { return std::tolower(letter); });

	if ((PHYSFS_FileType::PHYSFS_FILETYPE_DIRECTORY == file_type ) || (file_extension == "/" ))
	{
		return ModuleFileSystem::FileType::DIRECTORY;
	}

	if (
		file_extension == "png"
		|| file_extension == "tif"
		|| file_extension == "dds"
		|| file_extension == "tga"
		|| file_extension == "jpg"
		)
	{
		return ModuleFileSystem::FileType::TEXTURE;
	}
	if (
		file_extension == "fbx"
		|| file_extension == "ol"
		)
	{
		return ModuleFileSystem::FileType::MODEL;
	}
	if (file_extension == "" && PHYSFS_FileType::PHYSFS_FILETYPE_OTHER == file_type)
	{
		return ModuleFileSystem::FileType::ARCHIVE;
	}
	return ModuleFileSystem::FileType::UNKNOWN;
}


std::string ModuleFileSystem::GetFileExtension(const char *file_path) const
{
	std::string file_path_string = std::string(file_path);
	if (file_path_string.back() == '/') 
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

void ModuleFileSystem::GetAllFilesInPath(const std::string & path, std::vector<std::shared_ptr<File>> & files, bool directories_only) const
{
	char **files_array = PHYSFS_enumerateFiles(path.c_str());
	if (*files_array == NULL)
	{
		APP_LOG_ERROR("Error reading directory: %s", PHYSFS_getLastError());
		return;
	}
	char **i;
	for (i = files_array; *i != NULL; i++)
	{
		std::shared_ptr<File> new_file = std::make_shared<File>(path, *i);
		bool is_directory = new_file->file_type == FileType::DIRECTORY;
		if (IsValidFileName(*i) && ((directories_only && is_directory) || !directories_only))
		{
			files.push_back(new_file);
		}
	}
	PHYSFS_freeList(files_array);
}

std::shared_ptr<ModuleFileSystem::File> ModuleFileSystem::GetFileHierarchyFromPath(const std::string & path) const
{
	std::shared_ptr<File> new_file = std::make_shared<File>();
	new_file->file_path = path;
	GetAllFilesRecursive(new_file);
	return new_file;
}
void ModuleFileSystem::GetAllFilesRecursive(std::shared_ptr<File> root) const
{
	std::vector<std::shared_ptr<File>> files;
	GetAllFilesInPath(root->file_path, files, true);
	for (auto & file : files )
	{
		file->parent = root;
		root->childs.push_back(file);
		GetAllFilesRecursive(file);

	}
}

size_t ModuleFileSystem::GetNumberOfSubFolders(const std::string & path) const
{
	char **files_array = PHYSFS_enumerateFiles(path.c_str());
	if (files_array == NULL)
	{
		APP_LOG_ERROR("Error reading directory: %s", PHYSFS_getLastError());
		return 0;
	}
	char **i;
	size_t subFiles = 0;
	for (i = files_array; *i != NULL; i++)
	{
		std::shared_ptr<File> new_file = std::make_shared<File>(path, *i);
		bool is_directory = new_file->file_type == FileType::DIRECTORY;
		if (IsValidFileName(*i) && is_directory)
		{
		++subFiles;
		}
	}
	PHYSFS_freeList(files_array);
	return subFiles;
}


bool ModuleFileSystem::IsValidFileName(const char * file_name) const
{
	return std::strcmp(file_name, ".") && std::strcmp(file_name, "..");
}

void ModuleFileSystem::RefreshFilesHierarchy()
{
	root_file = GetFileHierarchyFromPath("Assets");
}

bool ModuleFileSystem::File::operator==(const ModuleFileSystem::File& compare)
{
	return this->filename == compare.filename && this->file_path == compare.file_path && this->file_type == compare.file_type;
};
ModuleFileSystem::File::File(const std::string & path, const std::string & name) {
	this->filename = name;
	this->file_path = path + "/" + name;
	PHYSFS_Stat file_info;
	if (PHYSFS_stat(this->file_path.c_str(), &file_info) == 0)
	{
		APP_LOG_ERROR("Error getting %s file info: %s", this->file_path.c_str(),PHYSFS_getLastError());
	}

	this->file_type = App->filesystem->GetFileType(filename.c_str(), file_info.filetype);
	this->filename_no_extension = this->filename.substr(0, this->filename.find_last_of("."));
}

ModuleFileSystem::File::File(const std::string & path) {

	std::string name = path.substr(path.find_last_of('/')+1, -1);
	std::string not_name_path = path.substr(0, path.find_last_of('/'));
	*this = File(not_name_path,name);
}


