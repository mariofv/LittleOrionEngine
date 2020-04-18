#ifndef _SCENEMANAGER_H_
#define _SCENEMANAGER_H_

#include "Filesystem/File.h"

#include <string>
#include <vector>


class Scene;
class Config;
class GameObject;
class Metafile;

class SceneManager
{
public:
	SceneManager() = default;
	~SceneManager() = default;
	static std::shared_ptr<Scene> Load(Metafile* metafile, const FileData& resource_data);

};

#endif