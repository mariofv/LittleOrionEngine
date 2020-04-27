#ifndef _SOUNDMANAGER_H_
#define _SOUNDMANAGER_H_

#include "Filesystem/File.h"
#include <memory>

class Sound;
class SoundManager
{

public:
	SoundManager() = default;
	~SoundManager() = default;
	static std::shared_ptr<Sound> Load(uint32_t uuid, const FileData& resource_data);
private:
	static void Init();
};

#endif 
