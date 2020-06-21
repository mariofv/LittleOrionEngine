#ifndef _MODULEAUDIO_H_
#define _MODULEAUDIO_H_

#include "Module/Module.h"

#include <AK/SoundEngine/Common/AkMemoryMgr.h>  //Memory Manager
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>  //Streaming Manager
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/AkModule.h>  

#include <memory>
class SoundBank;
class ComponentAudioSource;
class GameObject;

class ModuleAudio : public Module
{
public:
	ModuleAudio();
	~ModuleAudio();
	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	ComponentAudioSource* CreateComponentAudioSource();
	void RemoveComponentAudioSource(ComponentAudioSource* audio_source_to_remove);

	const AkGameObjectID main_sound_gameobject = 3;

private:
	std::vector<ComponentAudioSource*> audio_sources;

	AkMemSettings memory_manager_settings;
	AkStreamMgrSettings streaming_manager_settings;
	AkInitSettings init_settings;
	AkPlatformInitSettings platform_init_settings;
	AkDeviceSettings device_settings;
};

#endif //_MODULEAUDIO_H_