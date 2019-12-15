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
	unsigned int LoadCubemap(std::vector<std::string> faces_paths) const;

private:
	ILubyte* LoadImageData(const char* texture_path, int & width, int & height, int image_type) const;
private:
	const std::string LIBRARY_TEXTURES_FOLDER = ".//Library//Materials";
};
#endif // !_MATERIALIMPORTER_H_
