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

	static FileData Binarize(Scene* material);
	static std::shared_ptr<Scene> Load(uint32_t uuid, const FileData& resource_data);
	static uint32_t Create(const std::string& new_scene_path);
};

#endif