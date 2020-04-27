#include "SoundManager.h"
#include "ResourceManagement/Resources/Sound.h"

void SoundManager::Init()
{
	//LOAD WWISE INIT
}

std::shared_ptr<Sound> SoundManager::Load(uint32_t uuid, const FileData & resource_data)
{
	std::shared_ptr<Sound> new_sound = std::make_shared<Sound>(uuid, 0);

	return new_sound;
}
