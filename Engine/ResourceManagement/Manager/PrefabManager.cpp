#include "PrefabManager.h"

#include "Component/ComponentCamera.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentLight.h"
#include "Component/ComponentAnimation.h"
#include "Component/ComponentScript.h"

#include "Helper/Config.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"

#include "ResourceManagement/Importer/PrefabImporter.h"
#include "ResourceManagement/Metafile/Metafile.h"
#include "ResourceManagement/Resources/Prefab.h"

#include <algorithm>

FileData PrefabManager::Binarize(Prefab* prefab)
{
	return App->resources->prefab_importer->ExtractFromGameObject(prefab->GetRootGameObject());
}

std::shared_ptr<Prefab> PrefabManager::Load(Metafile* metafile, const FileData& resource_data)
{
	char* prefab_file_data = (char*)resource_data.buffer;

	std::string serialized_prefab_string = prefab_file_data;
	Config scene_config(serialized_prefab_string);

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
		created_game_object->original_prefab = true;
		gameObjects.emplace_back(std::move(created_game_object));
	}

	std::shared_ptr<Prefab> new_prefab = std::make_shared<Prefab>(metafile, std::move(gameObjects));
	return new_prefab;
}

uint32_t PrefabManager::CreateFromGameObject(Path& creation_folder_path, GameObject& gameobject)
{
	std::string prefab_file_name = gameobject.name + ".prefab";
	FileData prefab_data = App->resources->prefab_importer->ExtractFromGameObject(&gameobject);
	return App->resources->CreateFromData(prefab_data, creation_folder_path, prefab_file_name);
}

void PrefabManager::LoadBasicParameters(const Config& config, std::unique_ptr<GameObject> & loaded_gameObject)
{
	loaded_gameObject->UUID = config.GetUInt("UUID", 0);

	config.GetString("Name", loaded_gameObject->name, "GameObject");

	loaded_gameObject->SetStatic(config.GetBool("IsStatic", false));
	loaded_gameObject->SetEnabled(config.GetBool("Active", true));

	Config transform_config;
	config.GetChildConfig("Transform", transform_config);
	loaded_gameObject->transform.Load(transform_config);

	Config transform_2d_config;
	config.GetChildConfig("Transform2D", transform_2d_config);
	loaded_gameObject->transform_2d.Load(transform_2d_config);

}

void PrefabManager::CreateComponents(const Config& config, std::unique_ptr<GameObject> & loaded_gameObject)
{
	std::vector<Config> gameobject_components_config;
	config.GetChildrenConfig("Components", gameobject_components_config);

	for (unsigned int i = 0; i < gameobject_components_config.size(); ++i)
	{
		uint64_t component_type_uint = gameobject_components_config[i].GetUInt("ComponentType", 0);
		assert(component_type_uint != 0);

		Component::ComponentType component_type = static_cast<Component::ComponentType>(component_type_uint);

		Component *created_component;
		switch (component_type)
		{
		case Component::ComponentType::CAMERA:
			created_component = new ComponentCamera();
			break;

		case Component::ComponentType::MESH_RENDERER:
			created_component = new ComponentMeshRenderer();
			break;

		case Component::ComponentType::LIGHT:
			created_component = new ComponentLight();
			break;

		case Component::ComponentType::ANIMATION:
			created_component = new ComponentAnimation();
			break;

		case Component::ComponentType::SCRIPT:
			created_component = new ComponentScript();
			break;
		}
		created_component->owner = loaded_gameObject.get();
		created_component->Load(gameobject_components_config[i]);
		loaded_gameObject->components.push_back(created_component);
	}
}