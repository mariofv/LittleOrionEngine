#include "ModuleScene.h"

#include "Main/Application.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleModelLoader.h"
#include "ModuleRender.h"
#include "Component/ComponentCamera.h"
#include "Helper/Config.h"
#include "UI/Panel/PanelHierarchy.h"

#include <algorithm>
#include <stack>
#include "Brofiler/Brofiler.h"

bool ModuleScene::Init()
{
	root = new GameObject(0);
	return true;
}

update_status ModuleScene::Update()
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::Crimson);
	for (auto & game_object : game_objects_ownership)
	{
		game_object->Update();
		if(!game_object->IsStatic())
		{
			ComponentMesh* object_mesh = (ComponentMesh*)game_object->GetComponent(Component::ComponentType::MESH);
			if(object_mesh != nullptr)
				App->renderer->UpdateAABBTree(game_object.get());
		}
	}
	return update_status::UPDATE_CONTINUE;
}


bool ModuleScene::CleanUp()
{
	DeleteCurrentScene();
	return true;
}

GameObject* ModuleScene::CreateGameObject()
{
	std::string created_game_object_name = App->editor->hierarchy->GetNextGameObjectName();
	std::unique_ptr<GameObject> created_game_object = std::make_unique<GameObject>(created_game_object_name);
	created_game_object->SetParent(root);

	GameObject * created_game_object_ptr = created_game_object.get();
	game_objects_ownership.emplace_back(std::move(created_game_object));

	return created_game_object_ptr;
}

GameObject* ModuleScene::CreateChildGameObject(GameObject *parent)
{
	GameObject * created_game_object_ptr = CreateGameObject();
	parent->AddChild(created_game_object_ptr);

	return created_game_object_ptr;
}

void ModuleScene::RemoveGameObject(GameObject * game_object_to_remove)
{
	auto it = std::find_if(game_objects_ownership.begin(), game_objects_ownership.end(), [game_object_to_remove](auto const & game_object) 
	{
		return game_object_to_remove == game_object.get();
	});
	if (it != game_objects_ownership.end() || game_object_to_remove == root)
	{
		std::vector<GameObject*> children_to_remove;
		game_object_to_remove->Delete(children_to_remove);
		game_objects_ownership.erase(std::remove_if(begin(game_objects_ownership), end(game_objects_ownership), [children_to_remove](auto const &  game_object)
		{
			return std::find(begin(children_to_remove), end(children_to_remove), game_object.get()) != end(children_to_remove);
		}
		), end(game_objects_ownership));
	}
}

GameObject* ModuleScene::GetRoot() const
{
	return root;
}

GameObject* ModuleScene::GetGameObject(uint64_t UUID) const
{
	if (UUID == 0)
	{
		return root;
	}

	for (auto& game_object : game_objects_ownership)
	{
		if (game_object->UUID == UUID) 
		{
			return game_object.get();
		}
	}

	return nullptr;
}

Component * ModuleScene::GetComponent(uint64_t UUID) const
{
	for (auto& game_object : game_objects_ownership)
	{
		for(auto& component : game_object->components)
		{
			if(component->UUID == UUID)
			{
				return component;
			}
		}
	}

	return nullptr;
}

void ModuleScene::DeleteCurrentScene()
{
	//UndoRedo
	App->actions->ClearUndoRedoStacks();
	RemoveGameObject(root);
	App->renderer->DeleteAABBTree();
	App->editor->selected_game_object = nullptr;
}

void ModuleScene::Save(Config& serialized_scene) const
{
	std::vector<Config> game_objects_config(game_objects_ownership.size());
	std::stack<GameObject*> pending_objects;
	unsigned int current_index = 0;

	for (auto& child_game_object : root->children)
	{
		pending_objects.push(child_game_object);
	}	
	
	while (!pending_objects.empty())
	{
		GameObject* current_game_object = pending_objects.top();
		pending_objects.pop();

		current_game_object->Save(game_objects_config[current_index]);
		++current_index;

		for (auto& child_game_object : current_game_object->children)
		{
			pending_objects.push(child_game_object);
		}
	}
	assert(current_index == game_objects_ownership.size());

	serialized_scene.AddChildrenConfig(game_objects_config, "GameObjects");
}

void ModuleScene::Load(const Config& serialized_scene)
{
	DeleteCurrentScene();
	App->renderer->CreateAABBTree();
	root = new GameObject(0);

	std::vector<Config> game_objects_config;
	serialized_scene.GetChildrenConfig("GameObjects", game_objects_config);
	for (unsigned int i = 0; i < game_objects_config.size(); ++i)
	{
		GameObject* created_game_object = CreateGameObject();
		created_game_object->Load(game_objects_config[i]);

		if(!created_game_object->IsStatic())
		{
			App->renderer->InsertAABBTree(created_game_object);
		}
	}
	App->renderer->GenerateQuadTree();
	App->actions->ClearUndoStack();

}
