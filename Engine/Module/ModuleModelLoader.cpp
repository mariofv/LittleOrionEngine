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
#include "Component/ComponentMaterial.h"
#include "Component/ComponentMesh.h"

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

GameObject* ModuleModelLoader::LoadModel(const char *new_model_file_path) const
{

	File file(new_model_file_path);
	std::string model_output = App->resources->Import(file).second;

	GameObject *model_root_node = App->scene->CreateGameObject();
	model_root_node->name = std::string(file.filename_no_extension);

	std::vector<std::shared_ptr<File>> files_in_output_path;
	App->filesystem->GetAllFilesInPath(model_output, files_in_output_path);
	for (auto file : files_in_output_path )
	{
		LoadNode(model_root_node, file);
	}

	return model_root_node;
}

void ModuleModelLoader::LoadNode(GameObject *parent_node, const std::shared_ptr<File> & model_base_path) const
{
	GameObject *node_game_object = App->scene->CreateChildGameObject(parent_node);

	std::shared_ptr<Mesh> mesh_for_component = App->resources->Load<Mesh>(model_base_path->file_path.c_str());

	if (mesh_for_component == nullptr)
	{
		return;
	}

	ComponentMesh *mesh_component = (ComponentMesh*)node_game_object->CreateComponent(Component::ComponentType::MESH);
	mesh_component->SetMesh(mesh_for_component);
	node_game_object->name = model_base_path->filename_no_extension;
	node_game_object->Update();
	App->renderer->InsertAABBTree(node_game_object);

	ComponentMaterial *componentMaterial = (ComponentMaterial*)node_game_object->CreateComponent(Component::ComponentType::MATERIAL);
	for (auto texture : mesh_for_component->meshes_textures_path)
	{
		size_t separator = texture.find_last_of(":");
		std::string texture_path = texture.substr(separator+1, texture.size());
		std::string texture_type = texture.substr(0, separator);
		std::shared_ptr<Texture> texture = App->resources->Load<Texture>(texture_path);
		if (texture.get() != nullptr)
		{
			componentMaterial->SetMaterialTexture(std::stoi(texture_type), App->resources->Load<Texture>(texture_path));
		}
	}
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
	ComponentMesh* mesh_component = (ComponentMesh*)model_game_object->CreateComponent(Component::ComponentType::MESH);
	mesh_component->SetMesh(mesh_for_component);
	model_game_object->Update();

	ComponentMaterial* componentMaterial = (ComponentMaterial*)model_game_object->CreateComponent(Component::ComponentType::MATERIAL);
	for (auto texture : mesh_for_component->meshes_textures_path)
	{
		size_t separator = texture.find_last_of(":");
		std::string texture_path = texture.substr(separator + 1, texture.size());
		std::string texture_type = texture.substr(0, separator);
		componentMaterial->SetMaterialTexture(std::stoi(texture_type), App->resources->Load<Texture>(texture_path));
	}

	//UndoRedo
	App->actions->action_game_object = model_game_object;
	App->actions->AddUndoAction(ModuleActions::UndoActionType::ADD_GAMEOBJECT);

	return model_game_object;
}
