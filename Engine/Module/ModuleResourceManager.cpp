#include "ModuleResourceManager.h"
#include "Main/Application.h"

#include "ModuleFileSystem.h"
#include "Helper/Config.h"
#include "ResourceManagement/Importer/MaterialImporter.h"
#include "ResourceManagement/Importer/MeshImporter.h"

bool ModuleResourceManager::Init()
{
	APP_LOG_SECTION("************ Module Resource Manager Init ************");

	importing_thread = std::thread(&ModuleResourceManager::StartThread, this);
	return true;
}

update_status ModuleResourceManager::PreUpdate()
{
	return update_status::UPDATE_CONTINUE;
}

 bool ModuleResourceManager::CleanUp()
{
	 thread_comunication.stop_thread = true;
	 importing_thread.join();
	return true;
}


 std::pair<bool, std::string> ModuleResourceManager::Import(const File& file)
 {
	 while (thread_comunication.importing_hash == std::hash<std::string>{}(file.file_path))
	 {
		 Sleep(1000);
	 }
	 return InternalImport(file);
 }


 void ModuleResourceManager::StartThread()
 {
	 thread_comunication.finished_loading = false;
	 thread_comunication.total_items = App->filesystem->assets_file->total_sub_files_number;
	 ImportAllFileHierarchy(*App->filesystem->assets_file.get());
	 thread_comunication.finished_loading = true;
 }

void ModuleResourceManager::ImportAllFileHierarchy(const File& file)
 {
	 for (auto & child : file.children)
	 {
		 if (thread_comunication.stop_thread)
		 {
			 return;
		 }
		 thread_comunication.importing_hash = std::hash<std::string>{}(child->file_path);
		 if (child->file_type == FileType::DIRECTORY && !default_importer->Import(*child.get()).first)
		 {
			 ImportAllFileHierarchy(*child.get());
		 }
		 else if (child->file_type != FileType::DIRECTORY)
		 {
			 InternalImport(*child.get());
		 }
		 ++thread_comunication.loaded_items;
		 thread_comunication.importing_hash = 0;
	 }
 }


std::pair<bool, std::string> ModuleResourceManager::InternalImport(const File& file)
{
	std::pair<bool, std::string> result = std::pair<bool, std::string>(false,"");
	if (file.file_type == FileType::MODEL)
	{
		result = App->mesh_importer->Import(file);
	}
	if (file.file_type == FileType::TEXTURE)
	{
		result = App->material_importer->Import(file);
	}
	return result;
}



