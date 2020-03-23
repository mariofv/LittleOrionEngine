#include "ModuleFileSystem.h"

#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"

#include <algorithm>
#include <SDL/SDL.h>

bool ModuleFileSystem::Init()
{
	char* save_path = SDL_GetPrefPath("UPC", TITLE);
	if (save_path == NULL)
	{
		return false;
	}
	if (PHYSFS_setWriteDir(".") == 0)
	{
		APP_LOG_ERROR("Error creating writing directory: %s", PHYSFS_getLastError());
		return false;
	}

	MakeDirectory("Assets/Scenes");
	if (!CreateMountedDir("Assets"))
	{
		return false;
	}
	if (!CreateMountedDir("Resources"))
	{
		return false;
	}
	RefreshFilesHierarchy();
	return true;
}

bool ModuleFileSystem::CleanUp()
{
	return PHYSFS_deinit();
}

ModuleFileSystem::~ModuleFileSystem()
{
	CleanUp();
}

char* ModuleFileSystem::Load(const char* file_path, size_t & size) const
{ 
	assert(Exists(file_path));
	SDL_RWops *rw = SDL_RWFromFile(file_path, "rb");
	if (rw == NULL)
	{
		size = 0;
		return NULL;

	}

	size_t res_size = static_cast<size_t>(SDL_RWsize(rw));
	char* res = (char*)malloc(res_size + 1);

	size_t nb_read_total = 0, nb_read = 1;
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
	size = nb_read_total;
	return res;

}

bool ModuleFileSystem::Save(const char* file_path, const void* buffer, unsigned int size, bool append) const
{
	if (size == 0)
	{
		return false;
	}
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
		return false;
	}
	SDL_RWclose(file);
	return true;
}

bool ModuleFileSystem::Exists(const std::string& path) const
{
	return PHYSFS_exists(path.c_str());
}

bool ModuleFileSystem::Remove(const Path* path) const
{
	if (path == nullptr)
	{
		APP_LOG_ERROR("Nullptr path passed to Remove")
		return false;
	}

	bool success = PHYSFS_delete(path->file_path.c_str()) != 0;
	if (path->parent != nullptr)
	{
		path->parent->Refresh();
	}
	return success;
}

bool ModuleFileSystem::Remove(const std::string& path) const
{
	return Remove(&Path(path));
}

Path ModuleFileSystem::MakeDirectory(const std::string & new_directory_full_path) const
{
	if (PHYSFS_mkdir(new_directory_full_path.c_str()) == 0)
	{
		APP_LOG_ERROR("Error creating directory %s : %s", new_directory_full_path.c_str(), PHYSFS_getLastError());
		return Path();
	}
	return Path(new_directory_full_path);
}

bool ModuleFileSystem::Copy(const std::string& source, const std::string& destination)
{
	size_t file_size;
	if (!Exists(source))
	{
		APP_LOG_ERROR("Cannot copy file %s because it doesn't exist!", source.c_str())
		return false;
	}
	char* buffer = Load(source.c_str(), file_size);
	bool success = Save(destination.c_str(), buffer, file_size,false);
	free(buffer);
	return success;
}

void ModuleFileSystem::GetAllFilesInPath(const std::string& path, std::vector<std::shared_ptr<Path>>& files, bool directories_only) const
{
	char **files_array = PHYSFS_enumerateFiles(path.c_str());
	if (*files_array == NULL)
	{
		APP_LOG_INFO("Error reading directory: %s", PHYSFS_getLastError());
		return;
	}
	char **filename;
	for (filename = files_array; *filename != NULL; filename++)
	{
		std::shared_ptr<Path> new_file = std::make_shared<Path>(path, *filename);
		if (std::string(*filename).find(".meta") == std::string::npos)
		{
			bool is_directory = new_file->file_type == FileType::DIRECTORY;
			if ((directories_only && is_directory) || !directories_only)
			{
				files.push_back(new_file);
			}
		}
		else
		{
			std::string original_file = new_file->file_path.substr(0, new_file->file_path.find(".meta"));
			if (!Exists(original_file.c_str()))
			{
				ImportOptions options;
				Importer::GetOptionsFromMeta(new_file->file_path, options);
				if (!options.exported_file.empty())
				{
					Remove(&Path(options.exported_file));
				}
				Remove(new_file.get());
			}
			else
			{
				App->resources->resource_DB->AddEntry(*new_file);
			}
		}
	}
	PHYSFS_freeList(files_array);
}

void ModuleFileSystem::RefreshFilesHierarchy()
{
	assets_file = std::make_shared<Path>("Assets");
}


bool ModuleFileSystem::CreateMountedDir(const char* directory) const
{
	MakeDirectory(directory);
	if (PHYSFS_mount(directory, directory, 1) == 0)
	{
		APP_LOG_ERROR("Error mounting directory: %s", PHYSFS_getLastError());
		return false;
	}
	return true;
}

