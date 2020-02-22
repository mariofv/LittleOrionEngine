#ifndef _PREFABIMPORTER_H_
#define _PREFABIMPORTER_H_

#include "Importer.h"
class PrefabImporter : public Importer
{
public:
	PrefabImporter() = default;
	~PrefabImporter() = default;

	std::pair<bool, std::string> Import(const File & file) const override;
};

#endif