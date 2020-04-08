#include "ModuleScene.h"

#include "Component/ComponentCamera.h"
#include "EditorUI/Panel/PanelHierarchy.h"
#include "Helper/Config.h"
#include "Main/Application.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleModelLoader.h"
#include "ModuleRender.h"
#include "ModuleResourceManager.h"
#include "ModuleScriptManager.h"
#include "ModuleTime.h"

#include <algorithm>
#include <stack>
#include <Brofiler/Brofiler.h>

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
			ComponentMesh* object_mesh = (ComponentMesh*)game_object->GetComponent(Component::ComponentType::MESH_RENDERER);
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

ENGINE_API GameObject* ModuleScene::CreateGameObject()
{
	std::string created_game_object_name = App->editor->hierarchy->GetNextGameObjectName();
	std::unique_ptr<GameObject> created_game_object = std::make_unique<GameObject>(created_game_object_name);
	created_game_object->SetParent(root);

	GameObject * created_game_object_ptr = created_game_object.get();
	game_objects_ownership.emplace_back(std::move(created_game_object));
	return created_game_object_ptr;
}

ENGINE_API GameObject* ModuleScene::CreateChildGameObject(GameObject *parent)
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


GameObject* ModuleScene::AddGameObject(std::unique_ptr<GameObject> & game_object_to_add)
{
	game_objects_ownership.emplace_back(std::move(game_object_to_add));
	GameObject * game_object = game_objects_ownership.back().get();
	game_object->SetParent(root);
	if (!game_object->IsStatic())
	{
		App->renderer->InsertAABBTree(game_object);
	}
	return game_object;

}


GameObject* ModuleScene::GetRoot() const
{
	return root;
}

ENGINE_API GameObject* ModuleScene::GetGameObject(uint64_t UUID) const
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
	App->resources->CleanResourceCache();
	App->renderer->DeleteAABBTree();
	App->scripts->scripts.clear();
	App->editor->selected_game_object = nullptr;
}

void ModuleScene::OpenScene(const std::string &path)
{
	App->scene->DeleteCurrentScene();
	App->renderer->CreateAABBTree();
	root = new GameObject(0);

	App->resources->scene_manager->Load(path);

	if (App->time->isGameRunning())
	{
		App->scripts->InitScripts();
	}
	App->renderer->GenerateQuadTree();
	App->renderer->GenerateOctTree();
	App->actions->ClearUndoStack();
}

void ModuleScene::OpenPendingScene()
{
	OpenScene(scene_to_load);
	scene_to_load.clear();
}

void ModuleScene::LoadScene(const std::string &path)
{
	scene_to_load = path;
}

bool ModuleScene::HasPendingSceneToLoad() const
{
	return !scene_to_load.empty();
}

