#include "ModuleAudio.h"
#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Resources/SoundBank.h"

ModuleAudio::ModuleAudio()
{
	AK::MemoryMgr::GetDefaultSettings(memory_manager_settings);
	AK::StreamMgr::GetDefaultSettings(streaming_manager_settings);
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
	if (AK::SoundEngine::Init(&init_settings, &platform_init_settings) != AK_Success)
	{
		APP_LOG_ERROR("Could not initialize the Sound Engine.");
		return false;
	}
	init_sound_bank = SoundManager::Init();
	main_sound_bank = App->resources->Load<SoundBank>(3373416799); //HARDCODE FOR TESTING
	return init_sound_bank != nullptr;
}

update_status ModuleAudio::Update()
{
	if (!playing)
	{
		AK::SoundEngine::PostEvent(

			AK::EVENTS::PLAY_SOUND_01,      // Unique ID of the event
			gameObj                         // Associated game object ID

		);
	}
	AK::SoundEngine::RenderAudio();
	return update_status::UPDATE_CONTINUE;
}

bool ModuleAudio::CleanUp()
{
	return true;
}