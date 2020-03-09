#ifndef _TEXTURELOADER_H
#define _TEXTURELOADER_H

#include <memory>
#include <vector>
#include <string>
class Texture;

namespace DDS
{

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

}
class TextureLoader
{
public:
	TextureLoader() = default;
	~TextureLoader()= default;

	static std::shared_ptr<Texture> Load(const std::string& file_path);
	static unsigned int LoadCubemap(std::vector<std::string> faces_paths);
private:
	static std::vector<char> LoadCompressedDDS(const std::string& file_path, DDS::DDS_HEADER & dds_header);
	static std::vector<char> LoadImageData(const std::string& file_path, int & width, int & height);

};
#endif
