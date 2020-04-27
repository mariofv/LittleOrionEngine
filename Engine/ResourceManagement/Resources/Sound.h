#ifndef _SOUND_H_
#define _SOUND_H_

#include "Resource.h"
class Sound : public Resource
{
public:
	Sound() = default;
	Sound(uint32_t uuid, int banck_id);
	~Sound();


	int banck_id;
};

#endif