#include "TextureManager.h"

#include "Helper/Config.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"

#include "ResourceManagement/Metafile/Metafile.h"
#include "ResourceManagement/Metafile/MetafileManager.h"
#include "ResourceManagement/Resources/Texture.h"

#include <Brofiler/Brofiler.h>
#include <GL/glew.h>

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

std::shared_ptr<Texture> TextureManager::Load(uint32_t uuid, const FileData& resource_data)
{
	std::shared_ptr<Texture> loaded_texture;

	if (MetafileManager::GetUUIDExportedFile(uuid).find("_normal") != std::string::npos)
	{
		int width, height;
		std::vector<char> data = LoadImageData(MetafileManager::GetUUIDExportedFile(uuid), width, height);
		loaded_texture = std::make_shared<Texture>(uuid, data.data(), 0, width, height, true);
	}
	else
	{
		DDS::DDS_HEADER ddsHeader;
		std::vector<char> data = LoadCompressedDDS(resource_data, ddsHeader);
		if (data.size())
		{
			loaded_texture = std::make_shared<Texture>(uuid, data.data(), data.size(), ddsHeader.dwWidth, ddsHeader.dwHeight);
		}
	}


	return loaded_texture;
}


std::vector<char> TextureManager::LoadImageData(const std::string& file_path, int & width, int & height)
{
	std::vector<char> data;
	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);
	ilLoadImage(file_path.c_str());

	ILenum error;
	error = ilGetError();
	if (error == IL_COULD_NOT_OPEN_FILE)
	{
		APP_LOG_ERROR("Error loading texture %s. File not found", file_path.c_str());
		return data;
	}

	ILinfo ImageInfo;
	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}

	ILubyte * loaded_data = ilGetData();
	data.resize(ilGetInteger(IL_IMAGE_SIZE_OF_DATA));
	memcpy(&data.front(), loaded_data, data.size());
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	ilDeleteImages(1, &image);
	return data;
}

std::vector<char> TextureManager::LoadCompressedDDS(const FileData& resource_data, DDS::DDS_HEADER & dds_header)
{
	char * loaded_data = (char*)resource_data.buffer;
	size_t dds_content_size = resource_data.size;

	std::vector<char> data;
	if (loaded_data)
	{
		size_t dds_header_offset = sizeof(DDS::DDS_HEADER) + DDS::magic_number;
		size_t data_size = dds_content_size - dds_header_offset;
		memcpy(&dds_header, loaded_data + DDS::magic_number, sizeof(DDS::DDS_HEADER));
		data.resize(data_size);
		memcpy(&data.front(), loaded_data + dds_header_offset, data_size);
	}
	return data;
}