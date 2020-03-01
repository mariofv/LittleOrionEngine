#include "PrefabLoader.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleFileSystem.h"

#include "ResourceManagement/Resources/Prefab.h"

#include "Component/ComponentCamera.h"
#include "Component/ComponentMaterial.h"
#include "Component/ComponentMesh.h"
#include "Component/ComponentLight.h"


#include "Helper/Config.h"
#include <algorithm>

std::shared_ptr<Prefab> PrefabLoader::Load(const std::string& file_path)
{

	size_t readed_bytes;
	char* scene_file_data = App->filesystem->Load(file_path.c_str(), readed_bytes);
	std::string serialized_scene_string = scene_file_data;
	free(scene_file_data);

	Config scene_config(serialized_scene_string);

	uint32_t uid = std::hash<std::string>{}(file_path);

	std::vector<Config> game_objects_config;
	scene_config.GetChildrenConfig("GameObjects", game_objects_config);

	std::vector<std::unique_ptr<GameObject>> gameObjects;
	gameObjects.reserve(game_objects_config.size());

	for (unsigned int i = 0; i < game_objects_config.size(); ++i)
	{
		std::unique_ptr<GameObject> created_game_object = std::make_unique<GameObject>();
		uint64_t parent_UUID = game_objects_config[i].GetUInt("ParentUUID", 0);
		auto& it = std::find_if(gameObjects.begin(), gameObjects.end(), [parent_UUID](const std::unique_ptr<GameObject>&  gameObject) {
			return gameObject->UUID == parent_UUID;
		});
		if (it != gameObjects.end())
		{
			created_game_object->SetParent((*it).get());
		}
		LoadBasicParameters(game_objects_config[i], created_game_object);
		CreateComponents(game_objects_config[i], created_game_object);
		created_game_object->original_UUID = game_objects_config[i].GetUInt("UUID", 0);
		gameObjects.emplace_back(std::move(created_game_object));
	}

	std::shared_ptr<Prefab> new_prefab = std::make_shared<Prefab>(std::move(gameObjects), uid, file_path);
	return new_prefab;
}

void PrefabLoader::LoadBasicParameters(const Config& config, std::unique_ptr<GameObject> & loaded_gameObject)
{
	loaded_gameObject->UUID = config.GetUInt("UUID", 0);

	config.GetString("Name", loaded_gameObject->name, "GameObject");

	loaded_gameObject->SetStatic(config.GetBool("IsStatic", false));
	loaded_gameObject->SetEnabled(config.GetBool("Active", true));

	Config transform_config;
	config.GetChildConfig("Transform", transform_config);
	loaded_gameObject->transform.Load(transform_config);

}

void PrefabLoader::CreateComponents(const Config& config, std::unique_ptr<GameObject> & loaded_gameObject)
{
	std::vector<Config> gameobject_components_config;
	config.GetChildrenConfig("Components", gameobject_components_config);

	for (unsigned int i = 0; i < gameobject_components_config.size(); ++i)
	{
		uint64_t component_type_uint = gameobject_components_config[i].GetUInt("ComponentType", 0);
		assert(component_type_uint != 0);

		Component::ComponentType component_type = Component::GetComponentType(static_cast<unsigned int>(component_type_uint));

			Component *created_component;
			switch (component_type)
			{
			case Component::ComponentType::CAMERA:
				created_component = new ComponentCamera();
				break;

			case Component::ComponentType::MATERIAL:
				created_component = new ComponentMaterial();
				break;

			case Component::ComponentType::MESH:
				created_component = new ComponentMesh;
				break;

			case Component::ComponentType::LIGHT:
				created_component = new ComponentLight();
				break;
			}
			created_component->owner = loaded_gameObject.get();
			created_component->Load(gameobject_components_config[i]);
			loaded_gameObject->components.push_back(created_component);
	}
}