#include "SceneManager.h"
#include "Helper/Config.h"
#include <stack>
#include "Main/GameObject.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include <Module/ModuleScene.h>
#include <Module/ModuleRender.h>
#include <Module/ModuleResourceManager.h>
#include <unordered_map>

void SceneManager::Save(const std::string &path,  GameObject * gameobject_to_save) const
{

	Config scene_config;

	std::vector<Config> game_objects_config;
	std::vector<Config> prefabs_config;
	std::stack<GameObject*> pending_objects;

	if (gameobject_to_save->parent != nullptr)
	{
		Config current_gameobject;
		gameobject_to_save->Save(current_gameobject);
		game_objects_config.push_back(current_gameobject);
	}

	for (auto& child_game_object : gameobject_to_save->children)
	{
		pending_objects.push(child_game_object);
	}

	while (!pending_objects.empty())
	{
		GameObject* current_game_object = pending_objects.top();
		pending_objects.pop();


		if (current_game_object->is_prefab_parent)
		{
			Config current_prefab;
			SavePrefab(current_prefab, current_game_object);
			prefabs_config.push_back(current_prefab);
		}
		else {
			Config current_gameobject;
			current_game_object->Save(current_gameobject);
			game_objects_config.push_back(current_gameobject);
		}

		for (auto& child_game_object : current_game_object->children)
		{
			pending_objects.push(child_game_object);
		}
	}
	scene_config.AddChildrenConfig(prefabs_config, "Prefabs");
	scene_config.AddChildrenConfig(game_objects_config, "GameObjects");

	std::string serialized_scene_string;
	scene_config.GetSerializedString(serialized_scene_string);

	App->filesystem->Save(path.c_str(), serialized_scene_string.c_str(), serialized_scene_string.size() + 1);
}



void SceneManager::Load(const std::string &path) const
{

	size_t readed_bytes;
	char* scene_file_data = App->filesystem->Load(path.c_str(), readed_bytes);
	std::string serialized_scene_string = scene_file_data;
	free(scene_file_data);

	Config scene_config(serialized_scene_string);

	std::unordered_map<int64_t, GameObject*> prefab_parents;
	std::vector<Config> prefabs_config;
	scene_config.GetChildrenConfig("Prefabs", prefabs_config);
	for (unsigned int i = 0; i < prefabs_config.size(); ++i)
	{
		uint64_t parent_UUID = prefabs_config[i].GetUInt("ParentUUID", 0);
		GameObject * loaded_gameobject = LoadPrefab(prefabs_config[i]);
		if (parent_UUID != 0)
		{
			prefab_parents[parent_UUID] = loaded_gameobject;
		}
	}

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
		if (prefab_parents.find(created_game_object->UUID) != prefab_parents.end())
		{
			prefab_parents[created_game_object->UUID]->SetParent(created_game_object);
		}
	}
}


void SceneManager::SavePrefab(Config & config, GameObject * gameobject_to_save) const
{
	if (gameobject_to_save->parent != nullptr)
	{
		config.AddUInt(gameobject_to_save->parent->UUID, "ParentUUID");
	}
	config.AddString(gameobject_to_save->prefab_reference->exported_file, "Prefab");

	if (gameobject_to_save->is_prefab_parent || gameobject_to_save->transform.modified_by_user)
	{
		Config transform_config;
		gameobject_to_save->transform.Save(transform_config);
		config.AddChildConfig(transform_config, "Transform");
	}

	/*
	std::vector<Config> gameobject_components_config(components.size());
	for (unsigned int i = 0; i < components.size(); ++i)
	{
		components[i]->Save(gameobject_components_config[i]);
	}
	config.AddChildrenConfig(gameobject_components_config, "Components");*/
}

GameObject * SceneManager::LoadPrefab(const Config & config) const
{
	std::string prefab_path;
	config.GetString("Prefab", prefab_path, "");

	std::shared_ptr<Prefab> prefab = App->resources->Load<Prefab>(prefab_path);
	GameObject * instance = prefab->Instantiate(App->scene->GetRoot());
	Config transform_config;
	config.GetChildConfig("Transform", transform_config);
	instance->transform.Load(transform_config);
	return instance;
}