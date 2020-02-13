#include "ModuleModelLoader.h"
#include "Main/Globals.h"
#include "Main/Application.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleTexture.h"
#include "ModuleResourceManager.h"
#include "Main/GameObject.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentMaterial.h"
#include "Component/ComponentMesh.h"
#include <ResourceManagement/Importer/Importer.h>

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

	std::string uuid = Importer::GetUIDFromMeta(Importer::GetMetaFilePath(file));
	size_t readed_bytes;
	char* prefab_file_data = App->filesystem->Load(uuid.c_str(), readed_bytes);
	std::string serialized_prefab_string = prefab_file_data;
	free(prefab_file_data);

	Config prefab_config(serialized_prefab_string);

	GameObject *model_root_node = App->scene->CreateGameObject();
	model_root_node->name = std::string(file.filename_no_extension);


	std::vector<Config> game_objects_config;
	prefab_config.GetChildrenConfig("Node", game_objects_config);
	for (unsigned int i = 0; i < game_objects_config.size(); ++i)
	{
		LoadNode(model_root_node, game_objects_config[i]);
	}

	return model_root_node;
}

void ModuleModelLoader::LoadNode(GameObject *parent_node, const Config & node_config) const
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
	return model_game_object;
}
