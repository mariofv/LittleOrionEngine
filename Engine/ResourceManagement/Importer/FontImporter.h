#ifndef _FONTIMPORTER_H_
#define _FONTIMPORTER_H_

#include "ResourceManagement/Importer/Importer.h"
#include "ResourceManagement/Resources/Font.h"

class FontImporter : public Importer
{
public:
	FontImporter() : Importer(ResourceType::FONT) {};
	~FontImporter() = default;
	FileData ExtractData(Path& assets_file_path, const Metafile& metafile) const override;
};

#endif // !_FONTIMPORTER_H_
