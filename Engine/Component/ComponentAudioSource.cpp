#include "ComponentAudioSource.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleAudio.h"

ComponentAudioSource::ComponentAudioSource() : Component(nullptr, ComponentType::AUDIO_SOURCE), gameobject_source(UUID)
{

}

ComponentAudioSource::ComponentAudioSource(GameObject* owner) : Component(owner, ComponentType::AUDIO_SOURCE), gameobject_source(UUID)
{

}

void ComponentAudioSource::Init()
{
	if (AK::SoundEngine::RegisterGameObj(gameobject_source))
	{
		const math::float3 owner_transform = owner->transform.GetTranslation();
		sound_position.SetPosition( owner_transform.x, owner_transform.y, owner_transform.z );
		AK::SoundEngine::SetPosition(gameobject_source, sound_position);
	}
	else
	{
		APP_LOG_ERROR("Unable to register sound gameobject");
	}
}

void ComponentAudioSource::Update()
{

}

void ComponentAudioSource::Delete()
{
	AK::SoundEngine::UnregisterGameObj(gameobject_source);
}

Component* ComponentAudioSource::Clone(bool original_prefab) const
{
	ComponentAudioSource * created_component;
	if (original_prefab)
	{
		created_component = new ComponentAudioSource();
	}
	else
	{
		created_component = App->audio->CreateComponentAudioSource();
	}
	*created_component = *this;
	return created_component;
};

void ComponentAudioSource::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentAudioSource*>(component_to_copy) = *this;
}

void ComponentAudioSource::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddUInt((uint64_t)type, "ComponentType");
	config.AddBool(active, "Active");
}

void ComponentAudioSource::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	uint32_t state_machine_uuid = config.GetUInt("StateMachineResource", 0);
}