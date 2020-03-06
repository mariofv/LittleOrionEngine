#ifndef _SCENEMANAGER_H_
#define _SCENEMANAGER_H_

#include "Importer.h"

class GameObject;
class SceneManager
{
public:
	SceneManager() = default;
	~SceneManager() = default;

	void Save(const std::string &path, GameObject * gameobject_to_save) const;
	void Load(const std::string &path) const;

private:
	void SavePrefab(Config & config, GameObject * gameobject_to_save) const;
};

#endif