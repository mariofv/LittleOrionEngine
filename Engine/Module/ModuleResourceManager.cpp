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
		 if (child->file_type == FileType::DIRECTORY && !LookForMetaFile(*child.get()).first)
		 {
			 ImportAllFileHierarchy(*child.get());
		 }
		 else 
		 {
			 InternalImport(*child.get());
		 }
		 ++thread_comunication.loaded_items;
		 thread_comunication.importing_hash = 0;
	 }
 }


std::pair<bool, std::string> ModuleResourceManager::InternalImport(const File& file)
{
	std::pair<bool, std::string> result = LookForMetaFile(file);
	if (result.first)
	{
		return result;
	}
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

std::pair<bool, std::string> ModuleResourceManager::LookForMetaFile(const File& file)
{
	int extension_index = file.file_path.find_last_of(".");
	extension_index = extension_index != std::string::npos ? extension_index : file.file_path.size();
	std::string meta_file_path = file.file_path.substr(0, extension_index) + ".meta";

	File meta_file(meta_file_path);
	if (App->filesystem->Exists(meta_file_path.c_str()) && meta_file.modification_timestamp >= file.modification_timestamp) {


		GetUIDFromMeta(meta_file);

		return std::pair<bool, std::string>(true, "");
	}

	return std::pair<bool, std::string>(false, "");
}

uint32_t ModuleResourceManager::GetUIDFromMeta(const File& file)
{
	size_t readed_bytes;
	char* meta_file_data = App->filesystem->Load(file.file_path.c_str(), readed_bytes);
	std::string serialized_string = meta_file_data;
	free(meta_file_data);

	Config meta_config(serialized_string);
	return meta_config.GetUInt("UID",0);
}