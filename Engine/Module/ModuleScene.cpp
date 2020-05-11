#include "ModuleScene.h"

#include "Component/ComponentCamera.h"
#include "EditorUI/Panel/PanelHierarchy.h"
#include "Filesystem/PathAtlas.h"
#include "Helper/BuildOptions.h"
#include "Helper/Config.h"

#include "Main/Application.h"
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
	std::unique_ptr<GameObject> aux_copy_pointer = std::make_unique<GameObject>();
	aux_copy_pointer.get()->Duplicate(*game_object);
	GameObject* duplicated_go = App->scene->AddGameObject(aux_copy_pointer);
	duplicated_go->SetParent(parent_go);
	duplicated_go->SetTransform(game_object);
	duplicated_go->name += "(1)";

	if(game_object->is_prefab_parent)
	{
		game_object->prefab_reference->Duplicate(duplicated_go);
	}
	
	for (const auto go : game_object->children)
	{
		DuplicateGameObject(go, duplicated_go);
	}

	return duplicated_go;
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
	App->space_partitioning->ResetAABBTree();
	App->scripts->scripts.clear();
	App->editor->selected_game_object = nullptr;
}

void ModuleScene::OpenScene()
{
	App->scene->DeleteCurrentScene();
	root = new GameObject(0);

	GetSceneResource();

	if (App->time->isGameRunning())
	{
		App->scripts->InitScripts();
	}
	App->space_partitioning->GenerateQuadTree();
	App->space_partitioning->GenerateOctTree();
	App->actions->ClearUndoStack();
}

inline void ModuleScene::GetSceneResource()
{
	if (load_tmp_scene)
	{
		tmp_scene.get()->Load();
		return;
	}
	else if (build_options_position != -1)
	{
		if (!build_options->is_imported)
		{
			//Only gets here if no build options exists
			GetSceneFromPath(DEFAULT_SCENE_PATH);
		}
		else
		{
			current_scene = App->resources->Load<Scene>(build_options.get()->GetSceneUUID(build_options_position));
		}
	}
	else
	{
		int position = build_options.get()->GetPositionFromPath(scene_to_load);

		#if GAME
			assert(position != -1);
		#endif

		(position != -1)
			? current_scene = App->resources->Load<Scene>(build_options.get()->GetSceneUUID(position))
			: GetSceneFromPath(scene_to_load);
	}

	current_scene.get()->Load();
}

void ModuleScene::GetSceneFromPath(const std::string& path)
{
	Path* metafile_path = App->filesystem->GetPath(App->resources->metafile_manager->GetMetafilePath(path));
	Metafile* scene_metafile = App->resources->metafile_manager->GetMetafile(*metafile_path);
	assert(scene_metafile != nullptr);
	current_scene = App->resources->Load<Scene>(scene_metafile->uuid);
}

void ModuleScene::OpenPendingScene()
{
	OpenScene();
	scene_to_load.clear();
	build_options_position = -1;
	load_tmp_scene = false;
}

ENGINE_API void ModuleScene::LoadScene(const std::string &path)
{
	scene_to_load = path;
}

ENGINE_API void ModuleScene::LoadScene(unsigned position)
{
	build_options_position = position;
}

void ModuleScene::LoadScene()
{
	load_tmp_scene = true;
}

void ModuleScene::SaveScene()
{
	if(App->time->isGameRunning())
	{
		APP_LOG_INFO("You must stop play mode to save scene.");
		return;
	}

	App->resources->Save<Scene>(current_scene);
}

void ModuleScene::SaveTmpScene()
{
	App->resources->Save<Scene>(tmp_scene);
}

bool ModuleScene::HasPendingSceneToLoad() const
{
	return !scene_to_load.empty() || build_options_position != -1 || load_tmp_scene;
}

void ModuleScene::SetCurrentScene(uint32_t uuid)
{
	current_scene = App->resources->Load<Scene>(uuid);
}

