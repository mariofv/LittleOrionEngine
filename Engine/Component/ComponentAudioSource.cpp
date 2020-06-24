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
	StopAll();
	StopEvent("menu_select");
	AK::SoundEngine::UnregisterGameObj(gameobject_source);
	App->audio->RemoveComponentAudioSource(this);
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

unsigned long ComponentAudioSource::PlayEvent(const std::string & event_to_play)
{
	AkPlayingID playing_id = AK::SoundEngine::PostEvent(event_to_play.c_str(), gameobject_source);
	if (playing_id == AK_INVALID_PLAYING_ID)
	{
		APP_LOG_ERROR("Unable to post main event");
	}
	AkUInt32 event_id = AK::SoundEngine::GetIDFromString(event_to_play.c_str());
	event_playing_ids[event_id] = playing_id;
	return playing_id;
}

ENGINE_API void ComponentAudioSource::StopEvent(const std::string & event_to_stop)
{
	AkUInt32 event_id = AK::SoundEngine::GetIDFromString(event_to_stop.c_str());
	if (event_playing_ids.find(event_id) != event_playing_ids.end())
	{
		StopEvent(event_playing_ids[event_id]);
	}
}

ENGINE_API void ComponentAudioSource::StopEvent(unsigned long playing_id_to_stop)
{
	AK::SoundEngine::StopPlayingID(playing_id_to_stop);
}

ENGINE_API void ComponentAudioSource::StopAll()
{
	AK::SoundEngine::StopAll(gameobject_source);
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
	CloneBase(static_cast<Component*>(created_component));
	return created_component;
};

void ComponentAudioSource::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentAudioSource*>(component_to_copy) = *this;
}

void ComponentAudioSource::SpecializedSave(Config& config) const
{
	config.AddFloat(volume, "Volume");
	uint32_t soundbank_uuid = soundbank ? soundbank->GetUUID() : 0;
	config.AddUInt(soundbank_uuid, "SoundBank");
	config.AddBool(sound_3d, "3DSound");
}

void ComponentAudioSource::SpecializedLoad(const Config& config)
{
	volume = config.GetFloat("Volume", 1);
	sound_3d = config.GetBool("3DSound", false);
	uint32_t soundbank_uuid = config.GetUInt("SoundBank", 0);
	if (soundbank_uuid != 0)
	{
		SetSoundBank(soundbank_uuid);
	}
	SetVolume(volume);
}