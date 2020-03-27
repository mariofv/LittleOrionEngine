#include "PrefabImporter.h"

#include "Component/ComponentMeshRenderer.h"
#include "Helper/Config.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "Main/GameObject.h"

#include "ResourceManagement/Resources/Material.h"
#include "ResourceManagement/Resources/Mesh.h"
#include "ResourceManagement/Resources/Skeleton.h"

#include <stack>

FileData PrefabImporter::ExtractData(Path& file_path) const
{
	return file_path.GetFile()->Load();
}

FileData PrefabImporter::ExtractFromGameObject(GameObject* gameobject) const
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
	prefab_config.GetSerializedString(serialized_prefab_string);

	char* prefab_bytes = new char[serialized_prefab_string.size() + 1];
	memcpy(prefab_bytes, serialized_prefab_string.c_str(), serialized_prefab_string.size() + 1);

	FileData prefab_data{ prefab_bytes, serialized_prefab_string.size() + 1 };
	return prefab_data;
}

FileData PrefabImporter::ExtractFromModel(const Config& model_config) const
{
	std::vector<std::unique_ptr<GameObject>> game_objects;
	std::vector<std::unique_ptr<ComponentMeshRenderer>> mesh_renderer_components;
	std::vector<uint32_t> loaded_skeletons;

	std::unique_ptr<GameObject> model_root_node = std::make_unique<GameObject>();
	model_root_node->original_UUID = model_root_node->UUID;

	model_config.GetString("Name", model_root_node->name, "");

	std::vector<Config> game_objects_config;
	model_config.GetChildrenConfig("Node", game_objects_config);
	for (unsigned int i = 0; i < game_objects_config.size(); ++i)
	{
		ExcractGameObjectFromNode(model_root_node, game_objects_config[i], game_objects, mesh_renderer_components, loaded_skeletons);
	}

	std::vector<Config> animation_config;
	model_config.GetChildrenConfig("Animations", animation_config);
	for (auto animation : animation_config)
	{
		uint32_t animation_uuid;
		animation_uuid = animation.GetUInt("Animation", 0);
	}

	FileData prefab_data = ExtractFromGameObject(model_root_node.get());

	game_objects.clear();
	mesh_renderer_components.clear();

	return prefab_data;
}

//For now we are representing the animation sketelon in the hierarchy just for visualization and learning, but proabbly this will not be needed in the future
void PrefabImporter::ExcractGameObjectFromNode
(
	std::unique_ptr<GameObject>& parent_node,
	const Config& node_config,
	std::vector<std::unique_ptr<GameObject>>& game_objects,
	std::vector<std::unique_ptr<ComponentMeshRenderer>>& mesh_renderer_components,
	std::vector<uint32_t>& loaded_skeletons
) {
	game_objects.emplace_back(std::make_unique<GameObject>());
	GameObject * node_game_object = game_objects.back().get();
	node_game_object->parent = parent_node.get();
	parent_node->children.push_back(node_game_object);

	node_config.GetString("Name", node_game_object->name, "");
	node_game_object->original_UUID = node_game_object->UUID;

	uint32_t material_uuid;
	material_uuid = node_config.GetUInt("Material", 0);

	uint32_t mesh_uuid;
	mesh_uuid = node_config.GetUInt("Mesh", 0);

	if (mesh_uuid != 0)
	{
		ExcractMeshComponent(mesh_uuid, material_uuid, mesh_renderer_components, node_game_object);

		ComponentMeshRenderer* mesh_renderer = mesh_renderer_components.back().get();
		node_game_object->Update();
	}

	ExtractSkeletonFromNode(node_config, parent_node, game_objects, loaded_skeletons);
}


void PrefabImporter::ExcractMeshComponent(
	uint32_t mesh_uuid, uint32_t material_uuid,
	std::vector<std::unique_ptr<ComponentMeshRenderer>>& mesh_renderer_components,
	GameObject* node_game_object
) {
	mesh_renderer_components.emplace_back(std::make_unique<ComponentMeshRenderer>(node_game_object));
	node_game_object->components.push_back(mesh_renderer_components.back().get());

	mesh_renderer_components.back()->SetMesh(mesh_uuid);
	mesh_renderer_components.back()->SetMaterial(material_uuid);
}

void PrefabImporter::ExtractSkeletonFromNode(
	const Config& node_config, 
	std::unique_ptr<GameObject>& parent_node,
	std::vector<std::unique_ptr<GameObject>>& game_objects,
	std::vector<uint32_t>& loaded_skeletons
)
{
	uint32_t skeleton_uuid = node_config.GetUInt("Skeleton", 0);

	bool already_loaded = std::find(loaded_skeletons.begin(), loaded_skeletons.end(), skeleton_uuid) != loaded_skeletons.end();
	if (skeleton_uuid != 0 && !already_loaded)
	{
		std::shared_ptr<Skeleton> full_skeleton = std::static_pointer_cast<Skeleton>(App->resources->Load(skeleton_uuid));
		std::vector<GameObject*> skeleton_gameobjects;

		for (Skeleton::Joint joint : full_skeleton->skeleton)
		{
			game_objects.emplace_back(std::make_unique<GameObject>());
			GameObject* object = game_objects.back().get();
		
			if (joint.parent_index >= skeleton_gameobjects.size())
			{
				object->parent = parent_node.get();
				parent_node->children.push_back(object);
			}
			else
			{
				object->parent = skeleton_gameobjects.at(joint.parent_index);
				skeleton_gameobjects.at(joint.parent_index)->children.push_back(object);
			}

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

		loaded_skeletons.push_back(skeleton_uuid);
	}
}