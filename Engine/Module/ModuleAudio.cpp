#include "ModuleAudio.h"
#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Resources/SoundBank.h"
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
	AKRESULT eResult = AK::SoundEngine::LoadBank("Assets/Wwise/Init.bnk", init_banck_id);
	if (eResult != AK_Success)
	{
		APP_LOG_ERROR("Unable to load the init sound_bank");
	}
	eResult = AK::SoundEngine::LoadBank("Assets/Wwise/Play_main.bnk", banck_id);
	if (eResult != AK_Success)
	{
		APP_LOG_ERROR("Unable to load the sound_bank");
	}
	
	if (!AK::SoundEngine::RegisterGameObj(main_sound_gameobject))
	{
		APP_LOG_ERROR("Unable to register the gameobject");
	}
	/*const char * pszEvent = "play_main";
	eResult = AK::SoundEngine::PrepareEvent(AK::SoundEngine::Preparation_Load, &pszEvent, 1);
	if (eResult != AK_Success)
	{
		APP_LOG_ERROR("Could not initialize the bank.");
		return false;
	}*/
	return true;
}

update_status ModuleAudio::Update()
{
	if (!playing)
	{
		AkPlayingID playing_id = AK::SoundEngine::PostEvent(main_sound_event, main_sound_gameobject);
		if (playing_id == AK_INVALID_PLAYING_ID)
		{
			APP_LOG_ERROR("Unable to post main event");
		}
		playing = true;
	}
	AK::SoundEngine::RenderAudio();
	return update_status::UPDATE_CONTINUE;
}

bool ModuleAudio::CleanUp()
{
	AKRESULT eResult = AK::SoundEngine::UnloadBank(banck_id, NULL);
	if (eResult != AK_Success)
	{
		APP_LOG_ERROR("Unable to unload the sound_bank");
	}
	eResult = AK::SoundEngine::UnloadBank(init_banck_id, NULL);
	if (eResult != AK_Success)
	{
		APP_LOG_ERROR("Unable to unload the init sound_bank");
	}

	return true;
}