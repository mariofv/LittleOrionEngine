#include "ModuleFileSystem.h"

#include "Filesystem/PathAtlas.h"
#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"

#include <algorithm>
#include <SDL/SDL.h>
#include <stack>

ModuleFileSystem::~ModuleFileSystem()
{
	CleanUp();
}

bool ModuleFileSystem::Init()
{
	APP_LOG_SECTION("************ Module File System Init ************");

	//PHYSFS_permitSymbolicLinks(1);

	char* save_path = SDL_GetPrefPath("OnionGalaxy", TITLE);

	if (save_path == NULL)
	{
		return false;
	}

	if (PHYSFS_mount(".", "", 0) == 0)
	{
		APP_LOG_ERROR("Error mounting directory: %s", PHYSFS_getLastError());
		return false;
	}

	if (PHYSFS_setWriteDir(".") == 0)
	{
		APP_LOG_ERROR("Error creating writing directory: %s", PHYSFS_getLastError());
		return false;
	}
	
	CreatePathMap();

#if !GAME
	assets_folder_path = GetPath(ASSETS_PATH);
#endif
	resources_folder_path = GetPath(RESOURCES_PATH);

	if (!Exists(LIBRARY_PATH))
	{
		MakeDirectory(LIBRARY_PATH);
	}
	if (!Exists(LIBRARY_METADATA_PATH))
	{
		MakeDirectory(LIBRARY_METADATA_PATH);
	}
	library_folder_path = GetPath(LIBRARY_PATH);

	return true;
}

bool ModuleFileSystem::CleanUp()
{
	return PHYSFS_deinit();
}

Path* ModuleFileSystem::AddPath(const std::string& path)
{
	assert(Exists(path));
	assert(paths.find(path) == paths.end());

	Path* added_path = new Path(path);
	paths[path] = added_path;

	return added_path;
}

void ModuleFileSystem::RemovePath(Path* path)
{
	assert(!Exists(path->GetFullPath()));
	assert(paths.find(path->GetFullPath()) != paths.end());
	
	std::vector<Path*> pathes_to_delete;
	std::stack<Path*> pathes_to_check;
	pathes_to_check.push(path);
	while (!pathes_to_check.empty())
	{
		Path* current_path = pathes_to_check.top();
		pathes_to_check.pop();
		pathes_to_delete.push_back(current_path);

		for (auto& child : current_path->children)
		{
			pathes_to_check.push(child);
		}
	}
	
	for (auto& path_to_delete : pathes_to_delete)
	{
		paths.erase(path_to_delete->GetFullPath());
		delete path_to_delete;
	}
}

Path* ModuleFileSystem::GetPath(const std::string& path)
{
 	assert(Exists(path));
	assert(paths.find(path) != paths.end());
	return paths[path];
}

Path* ModuleFileSystem::GetRootPath() const
{
	return root_path;
}

FileData ModuleFileSystem::LoadFromSystem(const std::string & load_path)
{
	FileData loaded_data = {};

	SDL_RWops *rw = SDL_RWFromFile(load_path.c_str(), "rb");
	if (rw == NULL) 
		return loaded_data;

	size_t res_size = static_cast<size_t>(SDL_RWsize(rw));
	loaded_data.buffer = new char[res_size + 1];

	size_t nb_read_total = 0, nb_read = 1;
	char* buf = (char*) loaded_data.buffer;
	while (nb_read_total < res_size && nb_read != 0) {
		nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
		nb_read_total += nb_read;
		buf += nb_read;
	}
	SDL_RWclose(rw);
	if (nb_read_total != res_size) {
		delete[] loaded_data.buffer;
		return loaded_data;
	}

	((char*)loaded_data.buffer)[nb_read_total]= '\0';
	loaded_data.size = nb_read_total;
	return loaded_data;
}

Path* ModuleFileSystem::Save(const std::string& complete_save_path, FileData data_to_save)
{
	std::string save_path_folder;
	std::string save_path_filename;
	std::size_t found = complete_save_path.find_last_of("/");
	if (found == std::string::npos)
	{
		save_path_folder = "";
		save_path_filename = complete_save_path;
	}
	else if (found == 0) 
	{
		save_path_folder = "";
		save_path_filename = complete_save_path.substr(found + 1, -1);
	}
	else
	{
		save_path_folder = complete_save_path.substr(0, found);
		save_path_filename = complete_save_path.substr(found + 1, -1);
	}

	Path* saved_file_path = GetPath(save_path_folder);
	return saved_file_path->Save(save_path_filename.c_str(), data_to_save);
}

Path* ModuleFileSystem::Save(const std::string& complete_save_path, const std::string& serialized_data)
{
	std::string save_path_folder;
	std::string save_path_filename;
	std::size_t found = complete_save_path.find_last_of("/");
	if (found == std::string::npos)
	{
		save_path_folder = "";
		save_path_filename = complete_save_path;
	}
	else if (found == 0)
	{
		save_path_folder = "";
		save_path_filename = complete_save_path.substr(found + 1, -1);
	}
	else
	{
		save_path_folder = complete_save_path.substr(0, found);
		save_path_filename = complete_save_path.substr(found + 1, -1);
	}

	Path* saved_file_path = GetPath(save_path_folder);
	return saved_file_path->Save(save_path_filename.c_str(), serialized_data);
}


bool ModuleFileSystem::Exists(const std::string& path) const
{
	return PHYSFS_exists(path.c_str());
}

bool ModuleFileSystem::Remove(Path* path)
{
	assert(path != nullptr);
	if (path->IsMeta())
	{
		std::string imported_file_path = path->GetFullPathWithoutExtension();
		if (Exists(imported_file_path))
		{
			Path* imported_path = GetPath(imported_file_path);
			bool success = PHYSFS_delete(imported_file_path.c_str());
			if (success)
			{
				imported_path->parent->RemoveChild(imported_path);
				RemovePath(imported_path);
			}
		}
	}
	bool success = PHYSFS_delete(path->GetFullPath().c_str()) != 0;
	if (success)
	{
		path->parent->RemoveChild(path);
		RemovePath(path);
	}
	return success;
}

bool ModuleFileSystem::Remove(const std::string& path)
{
	Path* path_to_remove = App->filesystem->GetPath(path);
	return Remove(path_to_remove);
}

Path* ModuleFileSystem::MakeDirectory(const std::string& new_directory_full_path)
{
	if (Exists(new_directory_full_path))
	{
		return App->filesystem->GetPath(new_directory_full_path);
	}
	if (PHYSFS_mkdir(new_directory_full_path.c_str()) == 0)
	{
		APP_LOG_ERROR("Error creating directory %s : %s", new_directory_full_path.c_str(), PHYSFS_getLastError());
		return nullptr;
	}

	Path* created_dir = new Path(new_directory_full_path);
	Path* parent_path = GetPath(Path::GetParentPathString(new_directory_full_path));
	parent_path->children.push_back(created_dir);
	created_dir->parent = parent_path;
	paths[created_dir->GetFullPath()] = created_dir;
	return created_dir;
}

Path* ModuleFileSystem::Copy(const std::string& source_path, const std::string& destination_path, const std::string& copied_file_name)
{

	Path* source_path_object = GetPath(source_path);
	FileData source_file_data = source_path_object->GetFile()->Load();

	Path* destination_path_object = GetPath(destination_path);
	std::string file_name = copied_file_name == "" ? source_path_object->GetFilename() : copied_file_name;
	Path* copied_file_path = destination_path_object->Save(file_name.c_str(), source_file_data, false);
	
	return copied_file_path;
}

Path* ModuleFileSystem::Move(const std::string& source_path, const std::string& destination_path)
{
	Path* copied_file_path = Copy(source_path, destination_path);
	Remove(source_path);
	return copied_file_path;
}

Path * ModuleFileSystem::Rename(Path * file_to_rename, const std::string & new_name)
{
	if (file_to_rename->IsMeta())
	{
		Path* assets_file_path = App->filesystem->GetPath(file_to_rename->GetFullPathWithoutExtension());
		std::string assets_new_name = new_name.substr(0, new_name.find_last_of("."));
		App->filesystem->Copy(assets_file_path->GetFullPath(), assets_file_path->GetParent()->GetFullPath(), assets_new_name.substr());
		Path* new_metafile = App->filesystem->Copy(file_to_rename->GetFullPath(), file_to_rename->GetParent()->GetFullPath(), new_name);
		App->resources->metafile_manager->RefreshMetafile(*new_metafile);
		Remove(file_to_rename);
	}
	return nullptr;
}

bool ModuleFileSystem::CreateMountedDir(const std::string& directory)
{
	MakeDirectory(directory);
	return MountDirectory(directory);
}

bool ModuleFileSystem::MountDirectory(const std::string& directory) const
{
	if (PHYSFS_mount(directory.c_str(), directory.c_str(), 1) == 0)
	{
		APP_LOG_ERROR("Error mounting directory: %s", PHYSFS_getLastError());
		return false;
	}
	return true;
}

void ModuleFileSystem::CreatePathMap()
{
	delete root_path;

	paths.clear();

	root_path = new Path("");

	Path* current_path = nullptr;
	std::stack<Path*> remaining_paths_to_add;
	remaining_paths_to_add.push(root_path);

	while (!remaining_paths_to_add.empty())
	{
		 current_path = remaining_paths_to_add.top();
		 remaining_paths_to_add.pop();

		 paths[current_path->GetFullPath()] = current_path;
		 for (auto& path_child : current_path->children)
		 {
			 remaining_paths_to_add.push(path_child);
		 }
	}
}