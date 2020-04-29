#include "ModuleAudio.h"


ModuleAudio::ModuleAudio()
{
}


ModuleAudio::~ModuleAudio()
{
}

bool ModuleAudio::Init()
{
	return true;
}

update_status ModuleAudio::Update()
{
	return update_status::UPDATE_CONTINUE;
}

bool ModuleAudio::CleanUp()
{
	return true;
}