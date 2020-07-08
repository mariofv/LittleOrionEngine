#include "SoundBank.h"
#include "Main/Application.h"

#include <AK/SoundEngine/Platforms/Windows/AkTypes.h>
#include <memory>
SoundBank::SoundBank(uint32_t uuid, const void* data, size_t image_size) : game_obj(uuid),Resource(uuid)
{
	aligned_buffer_size = image_size + AK_BANK_PLATFORM_DATA_ALIGNMENT;
	aligned_buffer = new char[aligned_buffer_size];
	cursor = aligned_buffer + intptr_t(aligned_buffer) % AK_BANK_PLATFORM_DATA_ALIGNMENT;
	memcpy(cursor, data, image_size);
	AKRESULT eResult = AK::SoundEngine::LoadBankMemoryView(cursor, image_size, banck_id);
	if (eResult != AK_Success)
	{
		RESOURCES_LOG_ERROR("Unable to load the sound_bank");
	}
}
SoundBank::~SoundBank()
{
	AK::SoundEngine::UnloadBank(banck_id, cursor);
	delete[] aligned_buffer;
}