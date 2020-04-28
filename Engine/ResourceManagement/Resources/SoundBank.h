#ifndef _SOUNDBANK_H_
#define _SOUNDBANK_H_

#include "Resource.h"
#include "AK/SoundEngine/Common/AkSoundEngine.h"
class SoundBank : public Resource
{
public:
	SoundBank() = default;
	SoundBank(uint32_t uuid, const void* data, size_t image_size);
	~SoundBank();

private:
	AkBankID banck_id = 0;
	std::vector<char> data;
};

#endif