#ifndef _PREFABIMPORTER_H_
#define _PREFABIMPORTER_H_

#include "Importer.h"

class GameObject;
class PrefabImporter : public Importer
{
public:
	PrefabImporter() = default;
	~PrefabImporter() = default;
	ImportResult Import(const File & file, bool force = true) const override;
	void CreatePrefabResource(const File & file, GameObject * gameobject_to_save) const;

private:
	const std::string LIBRARY_TEXTURES_FOLDER = "Library/Prefabs";
};

#endif