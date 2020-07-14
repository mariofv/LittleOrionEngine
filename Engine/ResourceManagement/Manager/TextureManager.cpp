#include "TextureManager.h"

#include "Log/EngineLog.h"
#include "Helper/Config.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "Helper/Utils.h"

#include "ResourceManagement/Metafile/Metafile.h"
#include "ResourceManagement/Metafile/TextureMetafile.h"
#include "ResourceManagement/Metafile/MetafileManager.h"
#include "ResourceManagement/Resources/Texture.h"

#include <Brofiler/Brofiler.h>
#include <GL/glew.h>

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

constexpr size_t extension_size = 3; //3 characters: DDS, TGA, JPG..
std::shared_ptr<Texture> TextureManager::Load(uint32_t uuid, const FileData& resource_data)
{
	std::string extension;
	TextureOptions texture_options;
	char* cursor = (char*)resource_data.buffer;
	size_t bytes = sizeof(TextureOptions);
	memcpy(&texture_options, cursor, bytes);

	cursor += bytes;
	bytes = extension_size;
	extension.resize(extension_size);
	memcpy( extension.data(), cursor, bytes);

	size_t offset = extension_size + sizeof(TextureOptions);

	std::vector<char> data;
	int width, height, num_channels = 0;
	bool normal_map = texture_options.texture_type == TextureType::NORMAL;
	if (normal_map)
	{
		 data = LoadImageData(resource_data, offset, extension,width, height, num_channels);
	}
	else
	{
		DDS::DDS_HEADER ddsHeader;
		data = LoadCompressedDDS(resource_data, offset, ddsHeader);
		width = ddsHeader.dwWidth;
		height = ddsHeader.dwHeight;
	}
	std::shared_ptr<Texture> loaded_texture;
	if (data.size())
	{
		loaded_texture = std::make_shared<Texture>(uuid, data.data(), data.size(), width, height, num_channels, texture_options);
	}
	return loaded_texture;
}


std::vector<char> TextureManager::LoadImageData(const FileData& resource_data, size_t offset, const std::string& extension, int & width, int & height, int & num_channels)
{
	std::vector<char> data;
	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);
	char* resource_data_with_offset = (char*)resource_data.buffer + offset;
	ilLoadL(Utils::GetImageType(extension), resource_data_with_offset, resource_data.size - offset);
	
	ILenum error;
	error = ilGetError();
	while (error != IL_NO_ERROR)
	{
		RESOURCES_LOG_ERROR("Error loading texture: %s", iluErrorString(error));
		error = ilGetError();
	}

	ILinfo ImageInfo;
	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}

	ILubyte * loaded_data = ilGetData();
	data.resize(ilGetInteger(IL_IMAGE_SIZE_OF_DATA));
	num_channels = ilGetInteger(IL_IMAGE_CHANNELS);
	memcpy(&data.front(), loaded_data, data.size());
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	ilDeleteImages(1, &image);
	return data;
}

std::vector<char> TextureManager::LoadCompressedDDS(const FileData& resource_data, size_t offset, DDS::DDS_HEADER & dds_header)
{
	char * loaded_data = (char*)resource_data.buffer + offset;
	size_t dds_content_size = resource_data.size - offset;

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