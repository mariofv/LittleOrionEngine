#ifndef _SCENEIMPORTER_H_
#define _SCENEIMPORTER_H_

#include "ResourceManagement/Importer/Importer.h"

class SceneImporter : public Importer
{
public:
	SceneImporter() : Importer(ResourceType::SCENE) {};
	~SceneImporter() = default;

	FileData ExtractData(Path& assets_file_path, const Metafile& metafile) const override;
};

#endif //_SCENEIMPORTER_H_