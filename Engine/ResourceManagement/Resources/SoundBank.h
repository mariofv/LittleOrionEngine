#ifndef _SOUNDBANK_H_
#define _SOUNDBANK_H_

#include "Resource.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include "ResourceManagement/Manager/SoundManager.h"

class SoundBank : public Resource
{
public:
	SoundBank(uint32_t uuid, const void* data, size_t image_size);
	SoundBank(uint32_t uuid, const void* data, size_t image_size, std::vector<std::string> events);
	~SoundBank();

private:
	AkBankID bank_id = 0;
	AkGameObjectID game_obj = 0;
	char * aligned_buffer;
	char * cursor;
	size_t aligned_buffer_size = 0;
	std::vector<std::string> events;
};

namespace ResourceManagement
{
	template<>
	static std::shared_ptr<SoundBank> Load(uint32_t uuid, const FileData& resource_data, bool async)
	{
		return SoundManager::Load(uuid, resource_data);
	}
}

#endif