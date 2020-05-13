#include "ModuleAudio.h"
#include "Main/Application.h"
#include "Component/ComponentAudioSource.h"
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
	if (!AK::SoundEngine::RegisterGameObj(main_sound_gameobject))
	{
		APP_LOG_ERROR("Unable to register the gameobject");
	}
	AK::SoundEngine::AddDefaultListener(main_sound_gameobject);
	return true;
}

update_status ModuleAudio::Update()
{
	AK::SoundEngine::RenderAudio();
	return update_status::UPDATE_CONTINUE;
}

bool ModuleAudio::CleanUp()
{
	AK::SoundEngine::UnregisterGameObj(main_sound_gameobject);
	return true;
}

ComponentAudioSource * ModuleAudio::CreateComponentAudioSource()
{
	return new ComponentAudioSource();
}