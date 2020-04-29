#ifndef _MODULEAUDIO_H_
#define _MODULEAUDIO_H_

#include "Module/Module.h"

class ModuleAudio : public Module
{
public:
	ModuleAudio();
	~ModuleAudio();

	bool Init();
	update_status Update();
	bool CleanUp();
};

#endif //_MODULEAUDIO_H_