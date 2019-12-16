#include "SceneImporter.h"

void SceneImporter::LoadImportedFilesRegister() const
{
	char* buffer;
	App->filesystem->Load(REGISTER_PATH, &buffer);
	std::string full_register = std::string(buffer);
}

void SceneImporter::SaveImportedFilesRegister() const
{
	std::string full_register;
	for (auto & file_register : imported_file_register)
	{
		full_register += file_register.first + "," + file_register.second + "\n";
	}
	App->filesystem->Save(REGISTER_PATH, full_register.c_str(), sizeof(full_register));
}
