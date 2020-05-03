#include "ComponentAudioSource.h"

ComponentAudioSource::ComponentAudioSource() : Component(nullptr, ComponentType::AUDIO_SOURCE)
{

}

ComponentAudioSource::ComponentAudioSource(GameObject* owner) : Component(owner, ComponentType::AUDIO_SOURCE)
{

}

void ComponentAudioSource::Init()
{

}

void ComponentAudioSource::Update()
{

}

void ComponentAudioSource::Delete()
{
	
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