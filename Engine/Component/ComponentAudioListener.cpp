#include "ComponentAudioListener.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleAudio.h"
#include "Module/ModuleResourceManager.h"

ComponentAudioListener::ComponentAudioListener() : Component(nullptr, ComponentType::AUDIO_LISTENER), gameobject_listener(UUID)
{
	Init();
}


ComponentAudioListener::ComponentAudioListener(GameObject* owner) : Component(owner, ComponentType::AUDIO_LISTENER), gameobject_listener(UUID)
{
	Init();
}

void ComponentAudioListener::Init()
{
	if (!AK::SoundEngine::RegisterGameObj(gameobject_listener))
	{
		APP_LOG_ERROR("Unable to register sound gameobject");
	}
}

void ComponentAudioListener::Update()
{
	const math::float3 owner_transform = owner->transform.GetGlobalTranslation();
	listener_transform.SetPosition(owner_transform.x, owner_transform.y, owner_transform.z);

	const math::float3 front_vector = owner->transform.GetFrontVector();
	AkVector orientation_front{ front_vector.x, front_vector.y, -front_vector.z };	 

	const math::float3 top_vector = owner->transform.GetUpVector();
	AkVector orientation_top{ top_vector.x, top_vector.y, -top_vector.z };

	listener_transform.SetOrientation(orientation_front, orientation_top);

	AK::SoundEngine::SetPosition(gameobject_listener, listener_transform);

}

void ComponentAudioListener::Delete()
{
	AK::SoundEngine::UnregisterGameObj(gameobject_listener);
	App->audio->RemoveComponentAudioListener(this);
}

void ComponentAudioListener::SpecializedSave(Config& config) const
{
}

void ComponentAudioListener::SpecializedLoad(const Config& config)
{

}

AkGameObjectID ComponentAudioListener::GetAkID()
{
	return gameobject_listener;
}

Component* ComponentAudioListener::Clone(bool original_prefab) const
{
	ComponentAudioListener* created_component;
	if (original_prefab)
	{
		created_component = new ComponentAudioListener();
	}
	else
	{
		created_component = App->audio->CreateComponentAudioListener();
	}
	*created_component = *this;
	CloneBase(static_cast<Component*>(created_component));
	return created_component;
};

void ComponentAudioListener::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentAudioListener*>(component_to_copy) = *this;
}

void ComponentAudioListener::Disable()
{
	AK::SoundEngine::UnregisterGameObj(gameobject_listener);
}

void ComponentAudioListener::Enable()
{
	if (!AK::SoundEngine::RegisterGameObj(gameobject_listener))
	{
		APP_LOG_ERROR("Unable to register sound gameobject");
	}
}