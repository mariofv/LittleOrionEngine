#ifndef _PREFABIMPORTER_H_
#define _PREFABIMPORTER_H_

#include "Importer.h"

class GameObject;
class PrefabImporter : public Importer
{
public:
	PrefabImporter() = default;
	~PrefabImporter() = default;
	ImportResult Import(const Path& file, bool force = true) const override;
	void CreatePrefabResource(const Path& file, GameObject* gameobject_to_save) const;

};

#endif