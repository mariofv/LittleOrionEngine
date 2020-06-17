#include "ModuleScene.h"

#include "Component/ComponentCamera.h"
#include "EditorUI/Panel/PanelHierarchy.h"
#include "Filesystem/PathAtlas.h"
#include "Helper/BuildOptions.h"
#include "Helper/Config.h"

#include "Main/Application.h"
#include "ModuleAnimation.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleRender.h"
#include "ModuleResourceManager.h"
#include "ModuleScriptManager.h"
#include "Module/ModuleSpacePartitioning.h"
#include "ModuleTime.h"


#include "ResourceManagement/Metafile/Metafile.h"
#include "ResourceManagement/Metafile/MetafileManager.h"
#include "ResourceManagement/Manager/SceneManager.h"
#include "ResourceManagement/Resources/Scene.h"

#include <algorithm>
#include <stack>
#include <Brofiler/Brofiler.h>

bool ModuleScene::Init()
{
	root = new GameObject(0);
	build_options = std::make_unique<BuildOptions>();
	build_options->LoadOptions();

	Path* created_tmp = App->filesystem->Save(TMP_SCENE_PATH, std::string());
	App->resources->Import(*created_tmp);
	Path* metafile_path = App->filesystem->GetPath(App->resources->metafile_manager->GetMetafilePath(TMP_SCENE_PATH));
	Metafile* scene_metafile = App->resources->metafile_manager->GetMetafile(*metafile_path);
	assert(scene_metafile != nullptr);
	tmp_scene = App->resources->Load<Scene>(scene_metafile->uuid);



	return true;
}

update_status ModuleScene::PreUpdate()
{
	BROFILER_CATEGORY("Scene PreUpdate", Profiler::Color::Crimson);
	for (const auto& game_object : game_objects_ownership)
	{
		game_object->PreUpdate();
	}
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleScene::Update()
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::Crimson);
	for (const auto&  game_object : game_objects_ownership)
	{
		game_object->Update();
		App->space_partitioning->UpdateAABBTree(game_object.get());
	}
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate()
{
	BROFILER_CATEGORY("Scene PostUpdate", Profiler::Color::Crimson);
	for (const auto& game_object : game_objects_ownership)
	{
		game_object->PostUpdate();
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
	const auto it = std::find_if(game_objects_ownership.begin(), game_objects_ownership.end(), [game_object_to_remove](auto const& game_object)
	{
		return game_object_to_remove == game_object.get();
	});
	if (it != game_objects_ownership.end() || game_object_to_remove == root)
	{
		std::vector<GameObject*> children_to_remove;
		game_object_to_remove->Delete(children_to_remove);
		game_objects_ownership.erase(std::remove_if(begin(game_objects_ownership), end(game_objects_ownership), [&children_to_remove](auto const&  game_object)
		{
			return std::find(begin(children_to_remove), end(children_to_remove), game_object.get()) != end(children_to_remove);
		}
		), end(game_objects_ownership));
	}
}


GameObject* ModuleScene::AddGameObject(std::unique_ptr<GameObject>& game_object_to_add)
{
	game_objects_ownership.emplace_back(std::move(game_object_to_add));
	GameObject * game_object = game_objects_ownership.back().get();
	game_object->SetParent(root);
	if (!game_object->IsStatic())
	{
		App->space_partitioning->InsertAABBTree(game_object);
	}
	return game_object;

}

GameObject* ModuleScene::DuplicateGameObject(GameObject* game_object, GameObject* parent_go)
{

	//Duplicate code
	GameObject* clone_GO = DuplicateGO(game_object, parent_go);


	//Awake start scripts if needed
	if (App->time->isGameRunning())
	{
		GameObject* current_clone = clone_GO;
		std::stack<GameObject*> clone_stack;
		clone_stack.push(clone_GO);
		while (!clone_stack.empty()) 
		{
			current_clone = clone_stack.top();
			clone_stack.pop();

			InitDuplicatedScripts(current_clone);

			for (auto& go : current_clone->children) 
			{
				clone_stack.push(go);
			}		
		}
	}

	return clone_GO;
}

GameObject* ModuleScene::DuplicateGO(GameObject* game_object, GameObject* parent_go)
{
	std::unique_ptr<GameObject> aux_copy_pointer = std::make_unique<GameObject>();
	aux_copy_pointer.get()->Duplicate(*game_object);
	GameObject* duplicated_go = App->scene->AddGameObject(aux_copy_pointer);
	duplicated_go->SetParent(parent_go);
	duplicated_go->SetTransform(game_object);
	duplicated_go->name += "(1)";

	if (game_object->is_prefab_parent)
	{
		game_object->prefab_reference->Duplicate(duplicated_go);
	}

	for (const auto go : game_object->children)
	{
		DuplicateGO(go, duplicated_go);
	}

	return duplicated_go;
}

void ModuleScene::InitDuplicatedScripts(GameObject* clone_go)
{
	for (unsigned int i = 0; i < clone_go->components.size(); ++i)
	{
		if (clone_go->components[i]->type == Component::ComponentType::SCRIPT)
		{
			ComponentScript* script = (ComponentScript*)clone_go->components[i];
			if (!script->awaken)
			{
				script->AwakeScript();
				script->StartScript();
			}
		}
	}
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

	APP_LOG_INFO("Getting game object %u", UUID)
	APP_LOG_INFO("%d", game_objects_ownership.size())

	for (auto& game_object : game_objects_ownership)
	{		
		if (game_object->UUID == UUID) 
		{
			return game_object.get();
		}
	}

	return nullptr;
}

ENGINE_API GameObject* ModuleScene::GetGameObjectByName(const std::string & go_name) const
{
	APP_LOG_INFO("Getting game object %s", go_name.c_str());
	APP_LOG_INFO("%d", game_objects_ownership.size())

	for (auto& game_object : game_objects_ownership)
	{
		if (game_object->name == go_name)
		{
			return game_object.get();
		}
	}

	return nullptr;
}

GameObject* ModuleScene::GetGameObjectWithTag(const std::string& tag) const
{
	for (auto& game_object : game_objects_ownership)
	{
		if (game_object->tag == tag)
		{
			return game_object.get();
		}
	}

	return nullptr;
}

std::vector<GameObject*> ModuleScene::GetGameObjectsWithTag(const std::string& tag) const
{
	std::vector<GameObject*> returned_game_objects;
	for (auto& game_object : game_objects_ownership)
	{
		if (game_object->tag == tag)
		{
			returned_game_objects.push_back(game_object.get());
		}
	}

	return returned_game_objects;
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

void ModuleScene::OpenPendingScene()
{
	OpenScene();
	pending_scene_uuid = 0;
}

void ModuleScene::DeleteCurrentScene()
{
	//UndoRedo
	App->actions->ClearUndoRedoStacks();
	RemoveGameObject(root);
	App->space_partitioning->ResetAABBTree();
	App->scripts->scripts.clear();
	App->editor->selected_game_object = nullptr;
}

void ModuleScene::OpenScene()
{
	DeleteCurrentScene();
	root = new GameObject(0);

	LoadSceneResource();

	if (App->time->isGameRunning())
	{
		App->animations->PlayAnimations();
	}
	App->space_partitioning->GenerateQuadTree();
	App->space_partitioning->GenerateOctTree();
	App->actions->ClearUndoStack();
	App->time->ResetInitFrame();
}

inline void ModuleScene::LoadSceneResource()
{
	if (pending_scene_uuid == tmp_scene->GetUUID())
	{
		tmp_scene.get()->Load();
		current_scene = last_scene;
	}
	else if (pending_scene_uuid == GetSceneUUIDFromPath(DEFAULT_SCENE_PATH))
	{
		current_scene = nullptr;
		App->resources->Load<Scene>(pending_scene_uuid).get()->Load();
	}
	else
	{
		current_scene = App->resources->Load<Scene>(pending_scene_uuid);
		current_scene.get()->Load();
	}
}

uint32_t ModuleScene::GetSceneUUIDFromPath(const std::string& path)
{
	Path* metafile_path = App->filesystem->GetPath(App->resources->metafile_manager->GetMetafilePath(path));
	Metafile* scene_metafile = App->resources->metafile_manager->GetMetafile(*metafile_path);
	assert(scene_metafile != nullptr);
	return scene_metafile->uuid;
}

ENGINE_API void ModuleScene::LoadScene(const std::string &path)
{
	pending_scene_uuid = GetSceneUUIDFromPath(path);
}

ENGINE_API void ModuleScene::LoadScene(unsigned position)
{
	if (build_options->is_imported)
	{
		pending_scene_uuid = build_options->GetSceneUUID(position);
		if (pending_scene_uuid == 0)
		{
			OpenNewScene();
		}
	}
	else
	{
		OpenNewScene();
	}
}

void ModuleScene::SaveScene(uint32_t scene_uuid)
{
	if(App->time->isGameRunning())
	{
		APP_LOG_INFO("You must stop play mode to save scene.");
		return;
	}

	if (scene_uuid == 0) // Save current scene in its own resource
	{
		assert(current_scene != nullptr);
		App->resources->Save<Scene>(current_scene);
	}
	else // Save current scene in other resource
	{
		current_scene = App->resources->Load<Scene>(scene_uuid);
		App->resources->Save<Scene>(current_scene);
	}
}

void ModuleScene::OpenNewScene()
{
	LoadScene(DEFAULT_SCENE_PATH);
}

void ModuleScene::SaveTmpScene()
{
	App->resources->Save<Scene>(tmp_scene);
	last_scene = current_scene;
}

void ModuleScene::LoadTmpScene()
{
	LoadScene(TMP_SCENE_PATH);
}

bool ModuleScene::HasPendingSceneToLoad() const
{
	return pending_scene_uuid != 0;
}

bool ModuleScene::CurrentSceneIsSaved() const
{
	return current_scene != nullptr;
}
