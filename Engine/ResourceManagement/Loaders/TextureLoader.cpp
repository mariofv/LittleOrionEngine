#include "TextureLoader.h"
#include <Main/Application.h>
#include <Module/ModuleFileSystem.h>
#include <Module/ModuleResourceManager.h>
#include <GL/glew.h>
#include "Brofiler/Brofiler.h"
#include <ResourceManagement/Resources/Texture.h>
#include <Helper/Config.h>

char * TextureLoader::LoadCompressedDDS(const std::string& file_path, DDS::DDS_HEADER & dds_header, size_t & dds_content_size)
{
	char * data = App->filesystem->Load(file_path.c_str(), dds_content_size);
	if (data)
	{
		memcpy(&dds_header, data + DDS::magic_number, sizeof(DDS::DDS_HEADER));
	}
	return data;
}

unsigned int TextureLoader::LoadCubemap(std::vector<std::string> faces_paths)
{
	unsigned int texture_id;

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

	for (unsigned int i = 0; i < faces_paths.size(); i++)
	{
		size_t size;
		DDS::DDS_HEADER ddsHeader;
		char * data = LoadCompressedDDS(faces_paths[i].c_str(), ddsHeader, size);
		size_t dds_header_offset = sizeof(DDS::DDS_HEADER) + DDS::magic_number;
		if (data)
		{
			glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, ddsHeader.dwWidth, ddsHeader.dwHeight, 0, size - dds_header_offset, data + dds_header_offset);
			free(data);
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
		ILuint image;
		ilGenImages(1, &image);
		ilBindImage(image);
		int width, height;
		char * data = reinterpret_cast<char*>(App->resources->texture_importer->LoadImageData(file_path,IL_RGB, width, height));
		loaded_texture = std::make_shared<Texture>(data, 0,width, height, file_path);
		loaded_texture->normal_map = true;
		loaded_texture->Load(importing_options);
		ilDeleteImages(1, &image);
	}
	else
	{
		size_t size;
		DDS::DDS_HEADER ddsHeader;
		char * data = LoadCompressedDDS(file_path.c_str(), ddsHeader, size);
		if (data)
		{
			size_t dds_header_offset = sizeof(DDS::DDS_HEADER) + DDS::magic_number;
			loaded_texture = std::make_shared<Texture>(data + dds_header_offset, size - dds_header_offset, ddsHeader.dwWidth, ddsHeader.dwHeight, file_path);
			loaded_texture->Load(importing_options);
			free(data);
		}
	}


	return loaded_texture;
}