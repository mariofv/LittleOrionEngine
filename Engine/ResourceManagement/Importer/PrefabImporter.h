#ifndef _PREFABIMPORTER_H_
#define _PREFABIMPORTER_H_

#include "Importer.h"
class GameObject;
class PrefabImporter : public Importer
{
public:
	PrefabImporter() = default;
	~PrefabImporter() = default;
	std::pair<bool, std::string> Import(const File & file) const override;
	std::pair<bool, std::string> Import(const File & file, GameObject * gameobject_to_save) const;

private:
	const std::string LIBRARY_TEXTURES_FOLDER = "Library/Prefabs";
};

#endif