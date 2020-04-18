#ifndef _SKYBOXIMPORTER_H_
#define _SKYBOXIMPORTER_H_

#include "ResourceManagement/Importer/Importer.h"
#include "ResourceManagement/Resources/Skybox.h"

#include <assimp/material.h>
#include <vector>

struct aiScene;

class SkyboxImporter : public Importer
{
public:
	SkyboxImporter() : Importer(ResourceType::SKYBOX) {};
	~SkyboxImporter() = default;
	FileData ExtractData(Path& assets_file_path, const Metafile& metafile) const override;
};

#endif // !_SKYBOXIMPORTER_H_
