#include "PrefabManager.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentAudioListener.h"
#include "Component/ComponentAudioSource.h"
#include "Component/ComponentBillboard.h"
#include "Component/ComponentBoxCollider.h"
#include "Component/ComponentButton.h"
#include "Component/ComponentCapsuleCollider.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentCanvas.h"
#include "Component/ComponentCanvasRenderer.h"
#include "Component/ComponentCollider.h"
#include "Component/ComponentCylinderCollider.h"
#include "Component/ComponentEventSystem.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentLight.h"
#include "Component/ComponentMeshCollider.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentParticleSystem.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentSphereCollider.h"
#include "Component/ComponentTrail.h"
#include "Component/ComponentText.h"
#include "Component/ComponentVideoPlayer.h"

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

std::shared_ptr<Prefab> PrefabManager::Load(uint32_t uuid, const FileData& resource_data)
{
	char* prefab_file_data = (char*)resource_data.buffer;

	std::string serialized_prefab_string = prefab_file_data;
	Config scene_config(serialized_prefab_string);

	std::vector<Config> game_objects_config;
	scene_config.GetChildrenConfig("GameObjects", game_objects_config);

	std::vector<std::unique_ptr<GameObject>> gameObjects;
	gameObjects.reserve(game_objects_config.size());	
	
	bool overwritable = true;
	if (scene_config.config_document.HasMember("Overwritable"))
	{
		overwritable = scene_config.GetBool("Overwritable", true);
	}

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
		auto particles = gameObjects.back()->GetComponent(Component::ComponentType::PARTICLE_SYSTEM);
		if (particles)
		{
			assert(static_cast<ComponentParticleSystem*>(particles)->billboard->emissive_intensity > -1);
		}
	}


	std::shared_ptr<Prefab> new_prefab = std::make_shared<Prefab>(uuid, std::move(gameObjects), overwritable);
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
	config.GetString("Tag", loaded_gameObject->tag, "");

	loaded_gameObject->SetStatic(config.GetBool("IsStatic", false));
	loaded_gameObject->SetEnabled(config.GetBool("Active", true));

	Config transform_config;
	config.GetChildConfig("Transform", transform_config);
	loaded_gameObject->transform.Load(transform_config);

	Config transform_2d_config;
	config.GetChildConfig("Transform2D", transform_2d_config);
	loaded_gameObject->transform_2d.Load(transform_2d_config);

	loaded_gameObject->SetTransform2DStatus(config.GetBool("Transform2DEnabled", false));

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
		case Component::ComponentType::UI_IMAGE:
			created_component = new ComponentImage();
			break;
		case Component::ComponentType::VIDEO_PLAYER:
			created_component = new ComponentVideoPlayer();
			break;
		case Component::ComponentType::CANVAS:
			created_component = new ComponentCanvas();
			break;
		case Component::ComponentType::CANVAS_RENDERER:
			created_component = new ComponentCanvasRenderer();
			break;
		case Component::ComponentType::UI_BUTTON:
			created_component = new ComponentButton();
			break;
		case Component::ComponentType::AUDIO_SOURCE:
			created_component = new ComponentAudioSource();
			break;
		case Component::ComponentType::AUDIO_LISTENER:
			created_component = new ComponentAudioListener();
			break;
		case Component::ComponentType::PARTICLE_SYSTEM:
			created_component = new ComponentParticleSystem();
			break;
		case Component::ComponentType::BILLBOARD:
			created_component = new ComponentBillboard();
			break;
		case Component::ComponentType::EVENT_SYSTEM:
			created_component = new ComponentEventSystem();
			break;
		case Component::ComponentType::TRAIL:
			created_component = new ComponentTrail();
			break;
		case Component::ComponentType::UI_TEXT:
			created_component = new ComponentText();
			break;
		case Component::ComponentType::COLLIDER:
			ComponentCollider::ColliderType collider_type = static_cast<ComponentCollider::ColliderType>(gameobject_components_config[i].GetUInt("ColliderType", 0));
			switch (collider_type)
			{
			case ComponentCollider::ColliderType::BOX:
				created_component = new ComponentBoxCollider(loaded_gameObject.get());
				break;
			case ComponentCollider::ColliderType::CAPSULE:
				created_component = new ComponentCapsuleCollider(loaded_gameObject.get());
				break;
			case ComponentCollider::ColliderType::SPHERE:
				created_component = new ComponentSphereCollider(loaded_gameObject.get());
				break;
			case ComponentCollider::ColliderType::CYLINDER:
				created_component = new ComponentCylinderCollider(loaded_gameObject.get());
				break;
			case ComponentCollider::ColliderType::MESH:
				created_component = new ComponentMeshCollider(loaded_gameObject.get());
				break;
			}
			break;
		}
		if (component_type != Component::ComponentType::AABB && created_component)
		{
			created_component->owner = loaded_gameObject.get();
			created_component->Init();
			created_component->Load(gameobject_components_config[i]);
			loaded_gameObject->components.push_back(created_component);
		}
	}
}