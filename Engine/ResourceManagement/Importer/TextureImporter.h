#ifndef _TEXTUREIMPORTER_H_
#define _TEXTUREIMPORTER_H_

#include "Importer.h"
#include "ResourceManagement/Resources/Texture.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

class TextureMetafile;
class TextureImporter : public Importer
{

public:
	TextureImporter();
	~TextureImporter() = default;

	FileData ExtractData(Path& assets_file_path, const Metafile& metafile) const override;


private:
	FileData ExtractDataToDDS(const Path& assets_file_path) const;
	FileData ExtractDataToTGA(const Path& assets_file_path) const;

	ILubyte* LoadImageDataInMemory(const Path& file_path, int image_type, int& width, int& height) const;
	const FileData &CreateBinary(Path & assets_file_path, FileData &file_data, const TextureMetafile & texture_metafile) const;
};
#endif // !_TEXTUREIMPORTER_H_
