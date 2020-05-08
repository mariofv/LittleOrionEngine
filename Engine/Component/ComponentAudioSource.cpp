#include "ComponentAudioSource.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleAudio.h"
#include "Module/ModuleResourceManager.h"

ComponentAudioSource::ComponentAudioSource() : Component(nullptr, ComponentType::AUDIO_SOURCE), gameobject_source(UUID)
{
	Init();
}

ComponentAudioSource::ComponentAudioSource(GameObject* owner) : Component(owner, ComponentType::AUDIO_SOURCE), gameobject_source(UUID)
{
	Init();
}

void ComponentAudioSource::Init()
{
	if (!AK::SoundEngine::RegisterGameObj(gameobject_source))
	{
		APP_LOG_ERROR("Unable to register sound gameobject");
	}
}

void ComponentAudioSource::Update()
{
	if (sound_3d)
	{
		const math::float3 owner_transform = owner->transform.GetTranslation();
		sound_position.SetPosition(owner_transform.x, owner_transform.y, owner_transform.z);
		AK::SoundEngine::SetPosition(gameobject_source, sound_position);
	}
}

void ComponentAudioSource::Delete()
{

	AK::SoundEngine::UnregisterGameObj(gameobject_source);
}

void ComponentAudioSource::SetSoundBank(uint32_t uuid)
{
	soundbank = App->resources->Load<SoundBank>(uuid);
}

void ComponentAudioSource::SetVolume(float volume)
{
	this->volume = volume;
	AK::SoundEngine::SetGameObjectOutputBusVolume(gameobject_source,App->audio->main_sound_gameobject,volume);
}

void ComponentAudioSource::PlayEvent(const std::string & event_to_play)
{
	AkPlayingID playing_id = AK::SoundEngine::PostEvent(event_to_play.c_str(), gameobject_source);
	if (playing_id == AK_INVALID_PLAYING_ID)
	{
		APP_LOG_ERROR("Unable to post main event");
	}
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
	Component::Save(config);

	config.AddFloat(volume, "Volume");
	uint32_t soundbank_uuid = soundbank ? soundbank->GetUUID() : 0;
	config.AddUInt(soundbank_uuid, "SoundBank");
	config.AddBool(sound_3d, "3DSound");
}

void ComponentAudioSource::Load(const Config& config)
{
	Component::Load(config);
	volume = config.GetFloat("Volume", 1);
	sound_3d = config.GetBool("3DSound", false);
	uint32_t soundbank_uuid = config.GetUInt("SoundBank", 0);
	if (soundbank_uuid != 0)
	{
		SetSoundBank(soundbank_uuid);
	}
	SetVolume(volume);
}