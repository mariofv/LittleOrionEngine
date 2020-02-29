#include "PrefabLoader.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleFileSystem.h"

#include "ResourceManagement/Resources/Prefab.h"

#include "Helper/Config.h"
#include <algorithm>

std::shared_ptr<Prefab> PrefabLoader::Load(const std::string& file_path)
{

	size_t readed_bytes;
	char* scene_file_data = App->filesystem->Load(file_path.c_str(), readed_bytes);
	std::string serialized_scene_string = scene_file_data;
	free(scene_file_data);

	Config scene_config(serialized_scene_string);

	uint32_t uid = std::hash<std::string>{}(file_path);

	std::vector<Config> game_objects_config;
	scene_config.GetChildrenConfig("GameObjects", game_objects_config);

	std::vector<std::unique_ptr<GameObject>> gameObjects;
	gameObjects.reserve(game_objects_config.size());

	for (unsigned int i = 0; i < game_objects_config.size(); ++i)
	{
		std::unique_ptr<GameObject> created_game_object = std::make_unique<GameObject>();
		created_game_object->Load(game_objects_config[i]);
		uint64_t parent_UUID = game_objects_config[i].GetUInt("ParentUUID", 0);
		auto& it = std::find_if(gameObjects.begin(), gameObjects.end(), [parent_UUID](const std::unique_ptr<GameObject>&  gameObject) {
			return gameObject->UUID == parent_UUID;
		});
		if (it != gameObjects.end())
		{
			created_game_object->SetParent((*it).get());
		}

		gameObjects.emplace_back(std::move(created_game_object));
	}

	std::shared_ptr<Prefab> new_prefab = std::make_shared<Prefab>(std::move(gameObjects), uid, file_path);
	return new_prefab;
}