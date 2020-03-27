#include "ModuleModelLoader.h"
#include "Main/Globals.h"
#include "Main/Application.h"
#include "ModuleCamera.h"
#include "ModuleActions.h"
#include "ModuleScene.h"
#include "ModuleRender.h"
#include "ModuleTexture.h"
#include "ModuleResourceManager.h"
#include "Main/GameObject.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentMeshRenderer.h"
#include <ResourceManagement/Importer/Importer.h>

#include <ResourceManagement/Resources/Skeleton.h>
#include <ResourceManagement/Resources/Animation.h>
#include <ResourceManagement/ImportOptions/ImportOptions.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/mesh.h>

bool ModuleModelLoader::Init()
{
	APP_LOG_SECTION("************ Module ModelLoader Init ************");

	return true;
}

GameObject* ModuleModelLoader::LoadCoreModel(const char* new_model_file_path) const
{
	File file(new_model_file_path);
	GameObject* model_game_object = App->scene->CreateGameObject();
	model_game_object->name = std::string(file.filename_no_extension);

	std::shared_ptr<Mesh> mesh_for_component = App->resources->Load<Mesh>(file.file_path.c_str());
	if (mesh_for_component == nullptr)
	{
		return model_game_object;
	}

	ComponentMeshRenderer* mesh_renderer_component = (ComponentMeshRenderer*)model_game_object->CreateComponent(Component::ComponentType::MESH_RENDERER);
	mesh_renderer_component->SetMesh(mesh_for_component);
	mesh_renderer_component->SetMaterial(App->resources->Load<Material>(DEFAULT_MATERIAL_PATH));
	model_game_object->Update();

	//UndoRedo
	App->actions->action_game_object = model_game_object;
	App->actions->AddUndoAction(ModuleActions::UndoActionType::ADD_GAMEOBJECT);

	return model_game_object;
}
