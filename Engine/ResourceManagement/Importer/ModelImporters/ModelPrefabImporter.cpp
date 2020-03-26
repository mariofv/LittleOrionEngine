#include "ModelPrefabImporter.h"

#include "Component/ComponentMeshRenderer.h"
#include "Helper/Config.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleModelLoader.h"
#include "ResourceManagement/Resources/Animation.h"
#include "ResourceManagement/Resources/Skeleton.h"
#include "ResourceManagement/Resources/Mesh.h"
#include "ResourceManagement/Resources/Material.h"


static std::vector<std::unique_ptr<GameObject>> gameobjects;
static std::vector<std::unique_ptr<ComponentMeshRenderer>> mesh_renderer_components;

void ModelPrefabImporter::ImportModelPrefab(const Config& model, const File& imported_file) const
{
	std::unique_ptr<GameObject> model_root_node = std::make_unique<GameObject>();
	model_root_node->original_UUID = model_root_node->UUID;

	model.GetString("Name", model_root_node->name, "");

	std::vector<Config> game_objects_config;
	model.GetChildrenConfig("Node", game_objects_config);
	std::vector<std::string> already_loaded_skeleton;
	for (unsigned int i = 0; i < game_objects_config.size(); ++i)
	{
		LoadNode(model_root_node, game_objects_config[i], already_loaded_skeleton);
	}

	std::vector<Config> animation_config;
	model.GetChildrenConfig("Animations", animation_config);
	for (auto animation : animation_config)
	{
		std::string animation_uid;
		animation.GetString("Animation", animation_uid, "");
		std::shared_ptr<Animation> animation = App->resources->Load<Animation>(animation_uid);
	}

	App->resources->CreatePrefab(imported_file.file_path.c_str(), model_root_node.get());
	gameobjects.clear();
	mesh_renderer_components.clear();
}

//For now we are representing the animation sketelon in the hierarchy just for visualization and learning, but proabbly this will not be needed in the future
void ModelPrefabImporter::LoadNode(std::unique_ptr<GameObject> & parent_node, const Config& node_config, std::vector<std::string>& already_loaded_skeleton) const
{
	gameobjects.emplace_back(std::make_unique<GameObject>());
	GameObject * node_game_object = gameobjects.back().get();
	node_game_object->parent = parent_node.get();
	parent_node->children.push_back(node_game_object);

	std::string material_exported_file;
	node_config.GetString("Material", material_exported_file, "");
	std::string mesh_exported_file;
	node_config.GetString("Mesh", mesh_exported_file, "");

	if (mesh_exported_file != "")
	{
		LoadMeshComponent(mesh_exported_file, material_exported_file, node_game_object);

		ComponentMeshRenderer * mesh_renderer = mesh_renderer_components.back().get();
		node_config.GetString("Name", node_game_object->name, "");
		node_game_object->original_UUID = node_game_object->UUID;
		node_game_object->Update();
	}

	LoadSkeleton(node_config, already_loaded_skeleton, parent_node);
}

void ModelPrefabImporter::LoadMeshComponent(const std::string& mesh_exported_file, const std::string& material_exported_file, GameObject * node_game_object) const
{

	std::shared_ptr<Mesh> mesh_for_component = std::make_unique<Mesh>(mesh_exported_file);
	assert(mesh_for_component != nullptr);

	mesh_renderer_components.emplace_back(std::make_unique<ComponentMeshRenderer>(mesh_for_component, node_game_object));
	node_game_object->components.push_back(mesh_renderer_components.back().get());

	std::string material_file = material_exported_file != "" ? material_exported_file : DEFAULT_MATERIAL_PATH;

	std::shared_ptr<Material> material_resource = std::make_shared<Material>(0,DEFAULT_MATERIAL_PATH);
	mesh_renderer_components.back()->SetMaterial(material_resource);
}

void ModelPrefabImporter::LoadSkeleton(const Config& node_config, std::vector<std::string>& already_loaded_skeleton, std::unique_ptr<GameObject>& parent_node) const
{
	std::string skeleton_uid;
	node_config.GetString("Skeleton", skeleton_uid, "");

	bool already_loaded = std::find(already_loaded_skeleton.begin(), already_loaded_skeleton.end(), skeleton_uid) != already_loaded_skeleton.end();
	if (skeleton_uid != "" && !already_loaded)
	{
		std::shared_ptr<Skeleton> full_skeleton = App->resources->Load<Skeleton>(skeleton_uid.c_str());
		std::vector<GameObject*> skeleton_gameobjects;

		for (Skeleton::Joint joint : full_skeleton->skeleton)
		{
			gameobjects.emplace_back(std::make_unique<GameObject>());
			GameObject* object = gameobjects.back().get();
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
		already_loaded_skeleton.push_back(skeleton_uid);
	}
}
