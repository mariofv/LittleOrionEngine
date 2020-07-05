#include "PrefabImporter.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentTransform.h"
#include "Helper/Config.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "Main/GameObject.h"

#include "ResourceManagement/Metafile/ModelMetafile.h"
#include "ResourceManagement/Resources/Material.h"
#include "ResourceManagement/Resources/Mesh.h"
#include "ResourceManagement/Resources/Skeleton.h"

#include <stack>

FileData PrefabImporter::ExtractData(Path& file_path, const Metafile& metafile) const
{
	return file_path.GetFile()->Load();
}

FileData PrefabImporter::ExtractFromModel(const Config& model_config, const ModelMetafile& metafile, bool animated_model) const
{
	std::vector<std::unique_ptr<GameObject>> game_objects;
	std::vector<std::unique_ptr<ComponentMeshRenderer>> mesh_renderer_components;
	std::vector<uint32_t> loaded_skeletons;

	uint32_t real_uuid = metafile.uuid;
	std::unique_ptr<GameObject> model_root_node = std::make_unique<GameObject>();
	model_root_node->UUID = real_uuid;
	model_root_node->original_UUID = model_root_node->UUID;

	model_config.GetString("Name", model_root_node->name, "");

	std::vector<Config> game_objects_config;
	model_config.GetChildrenConfig("Node", game_objects_config);
	for (unsigned int i = 0; i < game_objects_config.size(); ++i)
	{
		ExtractGameObjectFromNode(model_root_node, game_objects_config[i], game_objects, mesh_renderer_components, loaded_skeletons, metafile, animated_model);
	}
	size_t gameobject_index = 1;
	for (auto & game_object : game_objects)
	{
		game_object->UUID = real_uuid + gameobject_index++;
		game_object->original_UUID = game_object->UUID;
	}

	ExtractAnimationComponent(model_root_node.get(), model_config);
	FileData prefab_data = ExtractFromGameObject(model_root_node.get(), false);

	game_objects.clear();
	mesh_renderer_components.clear();

	return prefab_data;
}

FileData PrefabImporter::ExtractFromGameObject(GameObject* gameobject, bool overwritable) const
{
	std::vector<Config> gameobjects_config;
	std::stack<GameObject*> pending_gameobjects;
	pending_gameobjects.push(gameobject);

	while (!pending_gameobjects.empty())
	{
		GameObject* current_gameobject = pending_gameobjects.top();
		pending_gameobjects.pop();

		Config current_gameobject_config;
		current_gameobject->Save(current_gameobject_config);
		gameobjects_config.push_back(current_gameobject_config);

		for (auto& child_gameobject : current_gameobject->children)
		{
			pending_gameobjects.push(child_gameobject);
		}
	}

	Config prefab_config;
	prefab_config.AddChildrenConfig(gameobjects_config, "GameObjects");

	std::string serialized_prefab_string;
	prefab_config.AddBool(overwritable, "Overwritable");
	prefab_config.GetSerializedString(serialized_prefab_string);

	char* prefab_bytes = new char[serialized_prefab_string.size() + 1];
	memcpy(prefab_bytes, serialized_prefab_string.c_str(), serialized_prefab_string.size() + 1);

	FileData prefab_data{ prefab_bytes, serialized_prefab_string.size() + 1 };
	return prefab_data;
}

void PrefabImporter::ExtractGameObjectFromNode
(
	std::unique_ptr<GameObject>& parent_node,
	const Config& node_config,
	std::vector<std::unique_ptr<GameObject>>& game_objects,
	std::vector<std::unique_ptr<ComponentMeshRenderer>>& mesh_renderer_components,
	std::vector<uint32_t>& loaded_skeletons,
	const ModelMetafile& metafile,
	bool animated_model
) {
	game_objects.emplace_back(std::make_unique<GameObject>());
	GameObject * node_game_object = game_objects.back().get();
	node_game_object->parent = parent_node.get();
	parent_node->children.push_back(node_game_object);

	node_config.GetString("Name", node_game_object->name, "");
	node_game_object->original_UUID = node_game_object->UUID;

	auto& remapped_material = metafile.remapped_materials;
	assert(remapped_material.find(node_game_object->name) != remapped_material.end());
	uint32_t remapped_material_uuid =  remapped_material.at(node_game_object->name);
	uint32_t material_uuid = remapped_material_uuid == 0 ? node_config.GetUInt32("Material", 0) : remapped_material_uuid;
	uint32_t mesh_uuid = node_config.GetUInt32("Mesh", 0);
	uint32_t skeleton_uuid = node_config.GetUInt32("Skeleton", 0);

	if (mesh_uuid != 0)
	{
		ExtractMeshComponent(mesh_uuid, material_uuid, skeleton_uuid, mesh_renderer_components, node_game_object);
		ComponentMeshRenderer* mesh_renderer = mesh_renderer_components.back().get();
		
		if (!animated_model)
		{
			float3 position = float3::zero;
			node_config.GetFloat3("Translation", position, float3::zero);
			node_game_object->transform.SetGlobalMatrixTranslation(position);

			Quat rotation;
			node_config.GetQuat("Rotation", rotation, Quat::identity);
			node_game_object->transform.SetGlobalMatrixRotation(rotation);

			node_config.GetFloat3("Scale", position, float3::one);
			node_game_object->transform.SetGlobalMatrixScale(position);

		}
		
		node_game_object->Update();
	}

}

void PrefabImporter::ExtractMeshComponent(
	uint32_t mesh_uuid, uint32_t material_uuid, uint32_t skeleton_uuid,
	std::vector<std::unique_ptr<ComponentMeshRenderer>>& mesh_renderer_components,
	GameObject* node_game_object
) {
	mesh_renderer_components.emplace_back(std::make_unique<ComponentMeshRenderer>(node_game_object));
	node_game_object->components.push_back(mesh_renderer_components.back().get());

	mesh_renderer_components.back()->mesh_uuid = mesh_uuid;
	mesh_renderer_components.back()->material_uuid = material_uuid;
	mesh_renderer_components.back()->skeleton_uuid = skeleton_uuid;
}

void PrefabImporter::ExtractAnimationComponent(GameObject* node_game_object, const Config& node_config)
{
	std::vector<Config> animation_config;
	node_config.GetChildrenConfig("Animations", animation_config);
	if (animation_config.size() > 0)
	{
		ComponentAnimation * component_animation = new ComponentAnimation();
		node_game_object->components.push_back(component_animation);
		component_animation->owner = node_game_object;
	}
}