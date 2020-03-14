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

void ModelPrefabImporter::ImportModelPrefab(const Config& model, const std::string& imported_file, std::string& exported_file) const
{
	std::unique_ptr<GameObject> model_root_node = std::make_unique<GameObject>();
	model_root_node->name = std::string("model_prefab"/*file.filename_no_extension*/);
	model_root_node->original_UUID = model_root_node->UUID;


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

	exported_file = App->resources->Import(imported_file, model_root_node.get()).exported_file;
}

//For now we are representing the animation sketelon in the hierarchy just for visualization and learning, but proabbly this will not be needed in the future
void ModelPrefabImporter::LoadNode(std::unique_ptr<GameObject> & parent_node, const Config& node_config, std::vector<std::string>& already_loaded_skeleton) const
{
	std::unique_ptr<GameObject> node_game_object = std::make_unique<GameObject>();
	node_game_object->SetParent(parent_node.get());



	std::string material_exported_file;
	node_config.GetString("Material", material_exported_file, "");
	std::string mesh_exported_file;
	node_config.GetString("Mesh", mesh_exported_file, "");

	if (mesh_exported_file != "")
	{
		std::unique_ptr<ComponentMeshRenderer> mesh_renderer = LoadMeshComponent(mesh_exported_file, material_exported_file, node_game_object.get());
		File file(mesh_renderer->mesh_to_render->exported_file);
		node_game_object->name = file.filename_no_extension;
		node_game_object->original_UUID = node_game_object->UUID;
		node_game_object->Update();
	}

	LoadSkeleton(node_config, already_loaded_skeleton, parent_node);
}

std::unique_ptr<ComponentMeshRenderer> ModelPrefabImporter::LoadMeshComponent(const std::string& mesh_exported_file, const std::string& material_exported_file, GameObject * node_game_object) const
{

	std::shared_ptr<Mesh> mesh_for_component = App->resources->Load<Mesh>(mesh_exported_file.c_str());
	assert(mesh_for_component != nullptr);

	std::unique_ptr<ComponentMeshRenderer> component_mesh_renderer = std::make_unique<ComponentMeshRenderer>();
	node_game_object->components.push_back(component_mesh_renderer.get());
	component_mesh_renderer->owner = node_game_object;
	component_mesh_renderer->SetMesh(mesh_for_component);

	std::string material_file = material_exported_file != "" ? material_exported_file : DEFAULT_MATERIAL_PATH;

	std::shared_ptr<Material> material_resource = App->resources->Load<Material>(material_file);
	component_mesh_renderer->SetMaterial(material_resource);

	return component_mesh_renderer;
}

void ModelPrefabImporter::LoadSkeleton(const Config& node_config, std::vector<std::string>& already_loaded_skeleton, std::unique_ptr<GameObject>& parent_node) const
{
	std::string skeleton_uid;
	node_config.GetString("Skeleton", skeleton_uid, "");

	bool already_loaded = std::find(already_loaded_skeleton.begin(), already_loaded_skeleton.end(), skeleton_uid) != already_loaded_skeleton.end();
	if (skeleton_uid != "" && !already_loaded)
	{
		std::shared_ptr<Skeleton> full_skeleton = App->resources->Load<Skeleton>(skeleton_uid.c_str());
		std::vector<std::unique_ptr<GameObject>> skeleton_gameobjects;

		for (Skeleton::Joint joint : full_skeleton->skeleton)
		{
			std::unique_ptr<GameObject> object = std::make_unique<GameObject>();
			std::unique_ptr<ComponentMeshRenderer> mesh_renderer = LoadMeshComponent(PRIMITIVE_CUBE_PATH, DEFAULT_MATERIAL_PATH, object.get());
			if (joint.parent_index >= skeleton_gameobjects.size())
			{
				object->SetParent(parent_node.get());
			}
			else
			{
				object->SetParent(skeleton_gameobjects.at(joint.parent_index).get());
			}

			float3 translation;
			float3 scale;
			float3x3 rotate;
			joint.transform_local.Decompose(translation, rotate, scale);

			object->transform.SetScale(scale);
			object->transform.SetTranslation(translation);
			object->transform.SetRotation(rotate);
			object->name = joint.name;

			skeleton_gameobjects.push_back(std::move(object));
		}
		already_loaded_skeleton.push_back(skeleton_uid);
	}
}
