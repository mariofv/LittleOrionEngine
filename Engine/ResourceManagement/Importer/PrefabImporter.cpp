#include "PrefabImporter.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Main/GameObject.h"
#include "Helper/Config.h"

#include <stack>

ImportResult PrefabImporter::Import(const File& file, bool force) const
{
	ImportResult import_result;

	if (file.filename.empty())
	{
		APP_LOG_ERROR("Importing material error: Couldn't find the file to import.")
			return import_result;
	}

	ImportOptions already_imported = GetAlreadyImportedResource(file);
	if (already_imported.uuid != 0 && !force) 
	{
		import_result.succes = true;
		import_result.exported_file = already_imported.exported_file;
		return import_result;
	}

	App->filesystem->MakeDirectory(LIBRARY_TEXTURES_FOLDER);

	std::string output_file = LIBRARY_TEXTURES_FOLDER + "/" + file.filename;
	bool copied = App->filesystem->Copy(file.file_path.c_str(), output_file.c_str());
	if (!copied)
	{
		return import_result;
	}
	SaveMetaFile(file.file_path, ResourceType::PREFAB, output_file);

	import_result.succes = true;
	import_result.exported_file = output_file;
	return import_result;
}

ImportResult PrefabImporter::Import(const File & file, GameObject * gameobject_to_save) const
{
	Config scene_config;

	std::vector<Config> game_objects_config;
	std::stack<GameObject*> pending_objects;


	Config current_gameobject;
	gameobject_to_save->Save(current_gameobject);
	game_objects_config.push_back(current_gameobject);

	for (auto& child_game_object : gameobject_to_save->children)
	{
		pending_objects.push(child_game_object);
	}

	while (!pending_objects.empty())
	{
		GameObject* current_game_object = pending_objects.top();
		pending_objects.pop();

		Config current_gameobject;
		current_game_object->Save(current_gameobject);
		game_objects_config.push_back(current_gameobject);

		for (auto& child_game_object : current_game_object->children)
		{
			pending_objects.push(child_game_object);
		}
	}
	scene_config.AddChildrenConfig(game_objects_config, "GameObjects");

	std::string serialized_scene_string;
	scene_config.GetSerializedString(serialized_scene_string);

	App->filesystem->Save(file.file_path.c_str(), serialized_scene_string.c_str(), serialized_scene_string.size() + 1);
	return Import(file);
}
