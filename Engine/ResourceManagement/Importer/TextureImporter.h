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
	~TextureImporter() = default;
	ImportResult Import(const File& file, bool force = false, bool skybox = false) const;
	std::string ImportMaterialData(const std::string& material_path, const std::string model_base_path) const;

private:
	std::string ImportToDDS(const File& imported_file, bool skybox = false) const;
	std::string ImportToTGA(const File& imported_file) const;
	ILubyte* LoadImageDataInMemory(const std::string& file_path, int image_type, int& width, int& height, bool skybox = false) const;
	std::string GetTextureFileName(std::string texture_file_path) const;

};
#endif // !_TEXTUREIMPORTER_H_
