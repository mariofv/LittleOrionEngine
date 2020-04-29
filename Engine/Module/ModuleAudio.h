#ifndef _MODULEAUDIO_H_
#define _MODULEAUDIO_H_

#include "Module/Module.h"

#include <AK/SoundEngine/Common/AkMemoryMgr.h>  //Memory Manager
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>  //Streaming Manager
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/AkModule.h>  

#include <memory>
class SoundBank;
class ModuleAudio : public Module
{
public:
	ModuleAudio();
	~ModuleAudio();

	bool Init();
	update_status Update();
	bool CleanUp();

	bool playing = false;
private:

	AkMemSettings memory_manager_settings;
	AkStreamMgrSettings streaming_manager_settings;
	AkInitSettings init_settings;
	AkPlatformInitSettings platform_init_settings;

	std::shared_ptr<SoundBank> init_sound_bank = nullptr;
	std::shared_ptr<SoundBank> main_sound_bank = nullptr;
};

#endif //_MODULEAUDIO_H_