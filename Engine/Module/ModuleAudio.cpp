#include "ModuleAudio.h"
#include "Main/Application.h"
#include "Component/ComponentAudioSource.h"
#include "Component/ComponentAudioListener.h"
#include "Module/ModuleResourceManager.h"
#include <SoundEngine/AkFilePackageLowLevelIOBlocking.h>
CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

ModuleAudio::ModuleAudio()
{
	AK::MemoryMgr::GetDefaultSettings(memory_manager_settings);
	AK::StreamMgr::GetDefaultSettings(streaming_manager_settings);
	AK::StreamMgr::GetDefaultDeviceSettings(device_settings);
	AK::SoundEngine::GetDefaultInitSettings(init_settings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platform_init_settings);
}


ModuleAudio::~ModuleAudio()
{
}

bool ModuleAudio::Init()
{
	APP_LOG_SECTION("************ Module Audio ************");
	if (AK::MemoryMgr::Init(&memory_manager_settings) != AK_Success)
	{
		APP_LOG_ERROR("Could not create the memory manager.");
		return false;
	}
	if (!AK::StreamMgr::Create(streaming_manager_settings))
	{
		APP_LOG_ERROR("Could not create the Streaming Manager");
		return false;
	}
	if (g_lowLevelIO.Init(device_settings) != AK_Success)
	{
		APP_LOG_ERROR("Could not create the streaming device and Low-Level I/O system");
		return false;
	}
	if (AK::SoundEngine::Init(&init_settings, &platform_init_settings) != AK_Success)
	{
		APP_LOG_ERROR("Could not initialize the Sound Engine.");
		return false;
	}

	return true;
}

update_status ModuleAudio::Update()
{
	SelectMainListener();
	AK::SoundEngine::RenderAudio();
	return update_status::UPDATE_CONTINUE;
}

bool ModuleAudio::CleanUp()
{
	AK::SoundEngine::UnregisterGameObj(default_listener);

	for (auto& audio_source : audio_sources)
	{
		audio_source->owner->RemoveComponent(audio_source);
	}
	audio_sources.clear();
	return true;
}

ComponentAudioSource * ModuleAudio::CreateComponentAudioSource()
{
	ComponentAudioSource * new_audio_source = new ComponentAudioSource();
	audio_sources.push_back(new_audio_source);
	return new_audio_source;
}

void ModuleAudio::RemoveComponentAudioSource(ComponentAudioSource* audio_source_to_remove)
{
	const auto it = std::find(audio_sources.begin(), audio_sources.end(), audio_source_to_remove);
	if (it != audio_sources.end())
	{
		delete *it;
		audio_sources.erase(it);
	}
}

ComponentAudioListener * ModuleAudio::CreateComponentAudioListener()
{
	ComponentAudioListener * new_audio_listener = new ComponentAudioListener();
	audio_listeners.push_back(new_audio_listener);
	return new_audio_listener;
}

void ModuleAudio::RemoveComponentAudioListener(ComponentAudioListener* audio_listener_to_remove)
{
	const auto it = std::find(audio_listeners.begin(), audio_listeners.end(), audio_listener_to_remove);
	if (it != audio_listeners.end())
	{
		delete *it;
		audio_listeners.erase(it);
	}
}

void ModuleAudio::SelectMainListener()
{
	main_listener = nullptr;

	for (auto& listener : audio_listeners)
	{
		if (listener->IsEnabled())
		{
			AkGameObjectID ak_id = listener->GetAkID();
			AK::SoundEngine::SetDefaultListeners(&ak_id, 1);
			main_listener = listener;
			return;
		}
	}
}