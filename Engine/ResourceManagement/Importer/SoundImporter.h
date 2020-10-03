#ifndef _SOUNDIMPORTER_H_
#define _SOUNDIMPORTER_H_

#include "Importer.h"

#include <map>

class SoundImporter : public Importer
{
public:
	SoundImporter() : Importer(ResourceType::SOUND) 
	{
		soundbank_events.clear(); 
	};
	~SoundImporter() = default;

	FileData ExtractData(Path& assets_file_path, const Metafile& metafile) const override;

private:
	void GenerateSoundBankEventsInfo() const;

public:
	mutable std::map<std::string, std::vector<std::string>> soundbank_events;
};

#endif