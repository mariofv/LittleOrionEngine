#include "Importer.h"

#include "Application.h"

std::shared_ptr<ModuleFileSystem::File> Importer::GetAlreadyImportedResource(const std::string & path_to_look_in, const ModuleFileSystem::File & file_to_look_for) const
{
	std::vector<std::shared_ptr<ModuleFileSystem::File>> resource_already_in_library;
	App->filesystem->GetAllFilesInPath(path_to_look_in, resource_already_in_library);

	//Check if the resource is already loaded
	auto it = std::find_if(resource_already_in_library.begin(), resource_already_in_library.end(), [file_to_look_for](const std::shared_ptr<ModuleFileSystem::File> & file)
	{
		return file->filename.find(file_to_look_for.filename_no_extension) != std::string::npos;
	});

	if (it != resource_already_in_library.end())
	{
		return *it;
	}
	return nullptr;
}