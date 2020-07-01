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
	const math::float3 owner_transform = owner->transform.GetTranslation();
	listener_transform.SetPosition(owner_transform.x, owner_transform.y, owner_transform.z);
	AK::SoundEngine::SetPosition(gameobject_listener, listener_transform);
}

void ComponentAudioListener::Delete()
{
	AK::SoundEngine::UnregisterGameObj(gameobject_listener);
	//App->audio->RemoveComponentAudioSource(this);
}
