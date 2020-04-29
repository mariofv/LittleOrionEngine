#include "SoundBank.h"
#include "Main/Application.h"
SoundBank::SoundBank(uint32_t uuid, const void* data, size_t image_size) : game_obj(uuid),Resource(uuid)
{

	AK::SoundEngine::RegisterGameObj(game_obj);
	this->data.resize(image_size);
	memcpy(&this->data.front(), data, image_size);
	AKRESULT eResult = AK::SoundEngine::LoadBankMemoryView(this->data.data(), image_size, banck_id);

	if (eResult != AK_Success)
	{
		APP_LOG_ERROR("Unable to load the sound_bank %d", uuid);
	}
}
SoundBank::~SoundBank()
{
	AK::SoundEngine::UnloadBank(banck_id, this->data.data());
}