#include "PrefabManager.h"
#include "Helper/Config.h"
#include <stack>
#include "Main/GameObject.h"
#include <Main/Application.h>
#include <Module/ModuleScene.h>
#include <Module/ModuleRender.h>

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



void PrefabManager::Load(const std::string &path) const
{

	size_t readed_bytes;
	char* scene_file_data = App->filesystem->Load(path.c_str(), readed_bytes);
	std::string serialized_scene_string = scene_file_data;
	free(scene_file_data);

	Config scene_config(serialized_scene_string);


	std::vector<Config> game_objects_config;
	scene_config.GetChildrenConfig("GameObjects", game_objects_config);
	for (unsigned int i = 0; i < game_objects_config.size(); ++i)
	{
		GameObject* created_game_object = App->scene->CreateGameObject();
		created_game_object->Load(game_objects_config[i]);

		if (!created_game_object->IsStatic())
		{
			App->renderer->InsertAABBTree(created_game_object);
		}
	}
}
