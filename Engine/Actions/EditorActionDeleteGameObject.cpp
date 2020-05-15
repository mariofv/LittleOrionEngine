#include "EditorActionDeleteGameObject.h"

#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleSpacePartitioning.h"
#include "Main/Application.h"
#include <stack>

EditorActionDeleteGameObject::EditorActionDeleteGameObject(GameObject* go) : UUID_go(go->UUID)
{
	std::vector<Config> game_objects_config(go->children.size() + 1);
	std::stack<GameObject*> pending_objects;
	unsigned int current_index = 1;

	Config root;
	go->Save(root);
	game_objects_config[0] = root;

	for(auto& child : go->children)
	{
		pending_objects.push(child);
	}

	while (!pending_objects.empty())
	{
		GameObject* current_game_object = pending_objects.top();
		pending_objects.pop();

		current_game_object->Save(game_objects_config[current_index]);
		++current_index;

		for (auto& child_game_object : current_game_object->children)
		{
			Config config;
			game_objects_config.push_back(config);
			pending_objects.push(child_game_object);
		}
	}

	serialized_gameobject.AddChildrenConfig(game_objects_config, "GameObjects");
}

void EditorActionDeleteGameObject::Undo()
{
	std::vector<Config> game_objects_config;
	serialized_gameobject.GetChildrenConfig("GameObjects", game_objects_config);
	for (unsigned int i = 0; i < game_objects_config.size(); ++i)
	{
		GameObject* created_game_object = App->scene->CreateGameObject();
		created_game_object->Load(game_objects_config[i]);

		if(UUID_go == created_game_object->UUID)
		{
			game_object = created_game_object;
		}

		if (!game_object->IsStatic())
			App->space_partitioning->InsertAABBTree(created_game_object);
	}
	
}

void EditorActionDeleteGameObject::Redo()
{
	App->scene->RemoveGameObject(game_object);
	App->editor->selected_game_object = nullptr;
}

