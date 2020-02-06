#ifndef _TEXTUREIMPORTER_H_
#define _TEXTUREIMPORTER_H_

#include "Importer.h"
#include "ResourceManagement/Resources/Texture.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include <assimp/material.h>
struct aiScene;

class TextureImporter : Importer
{

private:

	struct DDS_PIXELFORMAT {
		uint32_t dwSize;
		uint32_t dwFlags;
		uint32_t dwFourCC;
		uint32_t dwRGBBitCount;
		uint32_t dwRBitMask;
		uint32_t dwGBitMask;
		uint32_t dwBBitMask;
		uint32_t dwABitMask;
	};
	typedef struct {
		uint32_t           dwSize;
		uint32_t           dwFlags;
		uint32_t           dwHeight;
		uint32_t           dwWidth;
		uint32_t           dwPitchOrLinearSize;
		uint32_t           dwDepth;
		uint32_t           dwMipMapCount;
		uint32_t           dwReserved1[11];
		DDS_PIXELFORMAT ddspf;
		uint32_t           dwCaps;
		uint32_t           dwCaps2;
		uint32_t           dwCaps3;
		uint32_t           dwCaps4;
		uint32_t           dwReserved2;
	} DDS_HEADER;
	const uint32_t magic_number = 4;

public:
	TextureImporter();
	std::pair<bool, std::string> Import(const File & file) const override;
	void ImportMaterialFromMesh(const aiScene* scene, size_t mesh_index, const char* file_path,std::vector<std::string> & loaded_meshes_materials) const;
	std::string ImportMaterialData(const std::string& material_path, const std::string model_base_path) const;
	std::shared_ptr<Texture> Load(const std::string& file_path) const;
	unsigned int LoadCubemap(std::vector<std::string> faces_paths) const;

	void RemoveTextureFromCacheIfNeeded(const std::shared_ptr<Texture> & texture);
	Texture::TextureType GetTextureTypeFromAssimpType(aiTextureType type) const;

private:
	ILubyte* LoadImageData(const std::string& file_path, int image_type, int & width, int & height) const;
	char * LoadCompressedDDS(const std::string& file_path, DDS_HEADER & dds_header, size_t & dds_content_size) const;
	std::string GetTextureFileName(std::string texture_file_path) const;

private:
	const std::string LIBRARY_TEXTURES_FOLDER = "Library/Materials";
	mutable std::vector<std::shared_ptr<Texture>> texture_cache;

};
#endif // !_TEXTUREIMPORTER_H_
