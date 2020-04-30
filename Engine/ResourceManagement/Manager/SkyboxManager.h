#ifndef _SKYBOXMANAGER_H_
#define _SKYBOXMANAGER_H_

#include <memory>
#include <string>

#include "Filesystem/File.h"

class Skybox;
class Metafile;
class Path;

class SkyboxManager
{
public:
	SkyboxManager() = default;
	~SkyboxManager() = default;

	static FileData Binarize(Skybox* skybox);
	static std::shared_ptr<Skybox> Load(uint32_t uuid, const FileData& resource_data);
	static FileData Create();
};

#endif // !_MATERIALMANAGER_H_



