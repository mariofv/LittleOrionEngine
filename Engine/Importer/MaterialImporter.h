#ifndef _MATERIALIMPORTER_H_
#define _MATERIALIMPORTER_H_

#include "Importer/Importer.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

class Texture;

class MaterialImporter : Importer
{
public:
	MaterialImporter();
	bool Import(const char* file, std::string& output_file) const override;
	Texture* Load(const char* file)  const;
	ILubyte * LoadImageData(const char* texture_path, int & width, int & height, int image_type) const;
private:
private:
	const std::string LIBRARY_TEXTURES_FOLDER = ".//Library//Materials";
};
#endif // !_MATERIALIMPORTER_H_
