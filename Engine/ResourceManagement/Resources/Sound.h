#ifndef _SOUND_H_
#define _SOUND_H_

#include "Resource.h"
#include "AK/SoundEngine/Common/AkSoundEngine.h"
class Sound : public Resource
{
public:
	Sound() = default;
	Sound(uint32_t uuid, const void* data, size_t image_size);
	~Sound();

private:
	AkBankID banck_id = 0;
	std::vector<char> data;
};

#endif