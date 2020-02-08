#include <ResourceManagement/Resources/Texture.h>


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

char * LoadCompressedDDS(const std::string& file_path, DDS::DDS_HEADER & dds_header, size_t & dds_content_size)
{

}

std::shared_ptr<Texture> Loader::Load(const std::string& file_path) {

	size_t size;
	DDS::DDS_HEADER ddsHeader;
	char * data = LoadCompressedDDS(file_path.c_str(), ddsHeader, size);
	if (data)
	{
		size_t dds_header_offset = sizeof(DDS::DDS_HEADER) + DDS::magic_number;
		std::shared_ptr<Texture> loaded_texture = std::make_shared<Texture>(data + dds_header_offset, size - dds_header_offset, ddsHeader.dwWidth, ddsHeader.dwHeight, file_path);
		free(data);
		return loaded_texture;
	}
	return nullptr;
}

