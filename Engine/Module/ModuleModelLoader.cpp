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

GameObject* ModuleModelLoader::LoadModel(const char *new_model_file_path) const
{

	File file(new_model_file_path);
	ImportOptions options;
	Importer::GetOptionsFromMeta(Importer::GetMetaFilePath(file), options);
	size_t readed_bytes;
	char* prefab_file_data = App->filesystem->Load(options.uid.c_str(), readed_bytes);
	std::string serialized_prefab_string = prefab_file_data;
	free(prefab_file_data);

	Config prefab_config(serialized_prefab_string);

	GameObject *model_root_node = App->scene->CreateGameObject();
	model_root_node->name = std::string(file.filename_no_extension);


	std::vector<Config> game_objects_config;
	prefab_config.GetChildrenConfig("Node", game_objects_config);
	std::vector<std::string> already_loaded_skeleton;
	for (unsigned int i = 0; i < game_objects_config.size(); ++i)
	{
		LoadNode(model_root_node, game_objects_config[i], already_loaded_skeleton);
	}

	std::vector<Config> animation_config;
	prefab_config.GetChildrenConfig("Animations", animation_config);
	for (auto animation : animation_config)
	{
		std::string animation_uid;
		animation.GetString("Animation", animation_uid, "");
		std::shared_ptr<Animation> animation = App->resources->Load<Animation>(animation_uid);
	}

	return model_root_node;
}

//For now we are representing the animation sketelon in the hierarchy just for visualization and learning, but proabbly this will not be needed in the future
void ModuleModelLoader::LoadNode(GameObject *parent_node, const Config & node_config,  std::vector<std::string> & already_loaded_skeleton) const
{
	GameObject *node_game_object = App->scene->CreateChildGameObject(parent_node);
	std::string mesh_uid;
	node_config.GetString("Mesh", mesh_uid, "");

	if (mesh_uid != "")
	{
		std::shared_ptr<Mesh> mesh_for_component = App->resources->Load<Mesh>(mesh_uid.c_str());

		if (mesh_for_component == nullptr)
		{
			return;
		}

		ComponentMesh *mesh_component = (ComponentMesh*)node_game_object->CreateComponent(Component::ComponentType::MESH);
		mesh_component->SetMesh(mesh_for_component);
		File file(mesh_uid);
		node_game_object->name = file.filename_no_extension;
		node_game_object->Update();
		App->renderer->InsertAABBTree(node_game_object);

	}

	std::vector<Config> textures;
	node_config.GetChildrenConfig("Textures", textures);
	ComponentMaterial *componentMaterial = (ComponentMaterial*)node_game_object->CreateComponent(Component::ComponentType::MATERIAL);
	for (auto texture : textures)
	{
		std::string uid;
		texture.GetString("uid", uid, "");
		size_t separator = uid.find_last_of(":");
		std::string texture_path = uid.substr(separator+1, uid.size());
		std::string texture_type = uid.substr(0, separator);
		std::shared_ptr<Texture> texture_resource = App->resources->Load<Texture>(texture_path);
		if (texture_resource.get() != nullptr)
		{
			componentMaterial->SetMaterialTexture(std::stoi(texture_type), App->resources->Load<Texture>(texture_path));
		}
	}

	std::string skeleton_uid;
	node_config.GetString("Skeleton", skeleton_uid, "");

	bool already_loaded = std::find(already_loaded_skeleton.begin(), already_loaded_skeleton.end(), skeleton_uid) != already_loaded_skeleton.end();
	if (skeleton_uid != "" && !already_loaded)
	{
		std::shared_ptr<Skeleton> full_skeleton = App->resources->Load<Skeleton>(skeleton_uid.c_str());
		std::vector<GameObject *> skeleton_gameobjects;

		for (Skeleton::Joint joint : full_skeleton->skeleton)
		{
			GameObject * object = LoadCoreModel(PRIMITIVE_CUBE_PATH);

			if (joint.parent_index >= skeleton_gameobjects.size())
			{
				object->SetParent(parent_node);
			}
			else
			{
				object->SetParent(skeleton_gameobjects.at(joint.parent_index));
			}


			//I think animations comes in local position so for testing better use this

			float3 translation;
			float3 scale;
			float3x3 rotate;
			joint.transform_local.Decompose(translation, rotate, scale);

			object->transform.SetScale(scale);
			object->transform.SetTranslation(translation);
			object->transform.SetRotation(rotate);
			object->name = joint.name;

			skeleton_gameobjects.push_back(object);
		}
		already_loaded_skeleton.push_back(skeleton_uid);
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

	//UndoRedo
	App->actions->action_game_object = model_game_object;
	App->actions->AddUndoAction(ModuleActions::UndoActionType::ADD_GAMEOBJECT);

	return model_game_object;
}
