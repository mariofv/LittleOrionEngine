#include "TextureManager.h"

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

std::shared_ptr<Texture> TextureManager::Load(uint32_t uuid, const FileData& resource_data)
{

	std::string extension;
	TextureType texture_type;
	FileData texture_file_data;
	char* cursor = (char*)resource_data.buffer;
	size_t bytes = sizeof(TextureType);
	memcpy(&texture_type, cursor, bytes);

	cursor += bytes; // Store indices
	bytes = 3;
	extension.resize(3);
	memcpy( extension.data(), cursor, bytes);

	cursor += bytes; // Store vertices
	bytes = resource_data.size - 3 - sizeof(TextureType);
	char* texture_data = new char[bytes];
	texture_file_data.size = bytes;
	memcpy(texture_data, cursor, bytes);
	texture_file_data.buffer = texture_data;

	std::shared_ptr<Texture> loaded_texture;
	if (texture_type == TextureType::NORMAL)
	{
		int width, height;
		std::vector<char> data = LoadImageData(texture_file_data, extension,width, height);
		loaded_texture = std::make_shared<Texture>(uuid, data.data(), data.size(), width, height, true);
	}
	else
	{
		DDS::DDS_HEADER ddsHeader;
		std::vector<char> data = LoadCompressedDDS(texture_file_data, ddsHeader);
		if (data.size())
		{
			loaded_texture = std::make_shared<Texture>(uuid, data.data(), data.size(), ddsHeader.dwWidth, ddsHeader.dwHeight);
		}
	}


	return loaded_texture;
}


std::vector<char> TextureManager::LoadImageData(const FileData& resource_data,const std::string& file_path, int & width, int & height)
{
	std::vector<char> data;
	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);
	ilLoadL(Utils::GetImageType(file_path),resource_data.buffer, resource_data.size);

	ILenum error;
	error = ilGetError();
	while (error != IL_NO_ERROR)
	{
		APP_LOG_ERROR("Error loading texture %s. %s", file_path.c_str(),iluErrorString(error));
		error = ilGetError();
		//return data;
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