#include "PrefabManager.h"
#include "Helper/Config.h"
#include <stack>
#include "Main/GameObject.h"

void PrefabManager::Save(Config& serialized_scene, const GameObject * gameobject_to_save) const
{
	std::vector<Config> game_objects_config;
	std::stack<GameObject*> pending_objects;

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
	serialized_scene.AddChildrenConfig(game_objects_config, "GameObjects");
}