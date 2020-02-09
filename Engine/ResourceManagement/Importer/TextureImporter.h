#ifndef _TEXTUREIMPORTER_H_
#define _TEXTUREIMPORTER_H_

#include "Importer.h"
#include "ResourceManagement/Resources/Texture.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

class TextureImporter : Importer
{

public:
	TextureImporter();
	std::pair<bool, std::string> Import(const File & file) const override;
	std::string ImportMaterialData(const std::string& material_path, const std::string model_base_path) const;

private:
	ILubyte* LoadImageData(const std::string& file_path, int image_type, int & width, int & height) const;
	std::string GetTextureFileName(std::string texture_file_path) const;

private:
	const std::string LIBRARY_TEXTURES_FOLDER = "Library/Materials";

};
#endif // !_TEXTUREIMPORTER_H_
