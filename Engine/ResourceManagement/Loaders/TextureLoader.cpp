#include "TextureLoader.h"
#include <Main/Application.h>
#include <Module/ModuleFileSystem.h>
#include <GL/glew.h>
#include "Brofiler/Brofiler.h"
#include <ResourceManagement/Resources/Texture.h>
#include <Helper/Config.h>

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

std::vector<char> TextureLoader::LoadCompressedDDS(const std::string& file_path, DDS::DDS_HEADER & dds_header)
{
	size_t dds_content_size;
	char * loaded_data = App->filesystem->Load(file_path.c_str(), dds_content_size);
	std::vector<char> data;
	if (loaded_data)
	{
		size_t dds_header_offset = sizeof(DDS::DDS_HEADER) + DDS::magic_number;
		size_t data_size = dds_content_size - dds_header_offset;
		memcpy(&dds_header, loaded_data + DDS::magic_number, sizeof(DDS::DDS_HEADER));
		data.resize(data_size);
		memcpy(&data.front(), loaded_data + dds_header_offset, data_size);
	}
	free(loaded_data);
	return data;
}

unsigned int TextureLoader::LoadCubemap(std::vector<std::string> faces_paths)
{
	unsigned int texture_id;

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

	for (unsigned int i = 0; i < faces_paths.size(); i++)
	{
		DDS::DDS_HEADER ddsHeader;
		std::vector<char> data = LoadCompressedDDS(faces_paths[i].c_str(), ddsHeader);
		if (data.size() > 0)
		{
			glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, ddsHeader.dwWidth, ddsHeader.dwHeight, 0, data.size(), data.data());

		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texture_id;
}

std::shared_ptr<Texture> TextureLoader::Load(const std::string& file_path)
{

	if (!App->filesystem->Exists(file_path.c_str()))
	{
		return nullptr;
	}
	BROFILER_CATEGORY("Load Texture", Profiler::Color::BurlyWood);

	std::shared_ptr<Texture> loaded_texture;
	Config importing_options;
	if (file_path.find("_normal") != std::string::npos)
	{

		int width, height;
		std::vector<char> data = LoadImageData(file_path, width, height);
		loaded_texture = std::make_shared<Texture>(data.data(), 0,width, height, file_path, true);
	}
	else
	{
		DDS::DDS_HEADER ddsHeader;
		std::vector<char> data = LoadCompressedDDS(file_path.c_str(), ddsHeader);
		if (data.size())
		{
			loaded_texture = std::make_shared<Texture>(data.data(), data.size(), ddsHeader.dwWidth, ddsHeader.dwHeight, file_path);
		}
	}


	return loaded_texture;
}


std::vector<char> TextureLoader::LoadImageData(const std::string& file_path, int & width, int & height)
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