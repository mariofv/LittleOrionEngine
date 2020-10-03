#ifndef _SOUNDMANAGER_H_
#define _SOUNDMANAGER_H_

#include "Filesystem/File.h"
#include <memory>

class SoundBank;
class SoundManager
{
public:
	SoundManager() = default;
	~SoundManager() = default;
	static std::shared_ptr<SoundBank> Load(uint32_t uuid, const FileData& resource_data);
	static std::shared_ptr<SoundBank> Init();
};

#endif 
