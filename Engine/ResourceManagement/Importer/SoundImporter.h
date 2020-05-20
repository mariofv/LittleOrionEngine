#ifndef _SOUNDIMPORTER_H_
#define _SOUNDIMPORTER_H_

#include "Importer.h"
class SoundImporter : public Importer
{

public:
	SoundImporter() : Importer(ResourceType::SOUND) {};
	~SoundImporter() = default;

	FileData ExtractData(Path& assets_file_path, const Metafile& metafile) const override;


};

#endif