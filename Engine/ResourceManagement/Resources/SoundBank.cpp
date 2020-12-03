#include "SoundBank.h"

#include "Log/EngineLog.h"
#include "Main/Application.h"

#include <AK/SoundEngine/Platforms/Windows/AkTypes.h>
#include <memory>

SoundBank::SoundBank(uint32_t uuid, const void* data, size_t data_size) : game_obj(uuid), Resource(uuid)
{
	InitData(data_size, data);
}

SoundBank::SoundBank(uint32_t uuid, const void* data, size_t data_size, std::vector<std::string> events) :
	game_obj(uuid),Resource(uuid), events(events)
{
	InitData(data_size, data);
}

SoundBank::~SoundBank()
{
	AK::SoundEngine::UnloadBank(bank_id, cursor);
	delete[] aligned_buffer;
}

void SoundBank::InitData(size_t data_size, const void* data)
{
	RESOURCES_LOG_INFO("Init sound bank");
	assert(!aligned_buffer);
	aligned_buffer_size = data_size + AK_BANK_PLATFORM_DATA_ALIGNMENT;
	aligned_buffer = new char[aligned_buffer_size];
	cursor = aligned_buffer + intptr_t(aligned_buffer) % AK_BANK_PLATFORM_DATA_ALIGNMENT;
	memcpy(cursor, data, data_size);
	AKRESULT eResult = AK::SoundEngine::LoadBankMemoryView(cursor, data_size, bank_id);
	if (eResult != AK_Success)
	{
		RESOURCES_LOG_ERROR("Unable to load the sound_bank");
	}
}
