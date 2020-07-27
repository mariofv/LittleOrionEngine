#include "Texture.h"

#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Metafile/TextureMetafile.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

Texture::Texture(uint32_t uuid, char* data, size_t image_size, int width, int height, int num_channels, TextureOptions& options, bool async)
	: width(width), height(height), num_channels(num_channels)
	, Resource(uuid)
{
	this->data.resize(image_size);
	memcpy(&this->data.front(), data, image_size);

	this->texture_options.filter_mode = options.filter_mode;
	this->texture_options.generate_mipmaps = options.generate_mipmaps;
	this->texture_options.texture_type = options.texture_type;
	this->texture_options.wrap_mode = options.wrap_mode;

	if(!async)
	{
		LoadInMemory();
	}

}


Texture::~Texture()
{
	glDeleteTextures(1, &opengl_texture);
}

void Texture::LoadInMemory()
{
	if(initialized)
	{
		return;
	}


	glGenTextures(1, &opengl_texture);
	glBindTexture(GL_TEXTURE_2D, opengl_texture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	SetWrap(texture_options.wrap_mode);
	SetFilter(texture_options.filter_mode);
	
	GLint channels = num_channels > 3 ? GL_RGBA : GL_RGB;
	if (texture_options.texture_type == TextureType::NORMAL)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, channels, GL_UNSIGNED_BYTE, data.data());
	}
	else 
	{
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, width, height, 0, data.size(), data.data());
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	if (texture_options.generate_mipmaps)
	{
		GenerateMipMap();
	}

	initialized = true;
	++App->resources->loading_thread_communication.number_of_textures_loaded;
}

void Texture::GenerateMipMap()
{
	glBindTexture(GL_TEXTURE_2D, opengl_texture);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetWrap(WrapMode wrap)
{
	switch (wrap)
	{
	case REPEAT:
		this->wrap = GL_REPEAT;
		break;
	case CLAMP:
		this->wrap = GL_CLAMP;
		break;
	case MIRROR:
		this->wrap = GL_MIRRORED_REPEAT;
		break;
	case MIRROR_ONCE:
		this->wrap = GL_MIRROR_CLAMP_TO_EDGE;
		break;
	default:
		this->wrap = GL_REPEAT;
		break;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrap);
}

GLenum Texture::GetWrap() const
{
	return wrap;
}

char* Texture::GetWrap_C_Str() const
{
	return GLEnumToString(wrap);
}

void Texture::SetFilter(FilterMode filter)
{
	switch (filter)
	{
	case NEAREST:
		this->filter = GL_NEAREST;
		break;
	case LINEAR:
		this->filter = GL_LINEAR;
	default:
		break;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filter);

}

GLenum Texture::GetFilter() const
{
	return filter;
}

char* Texture::GetFilter_C_Str() const
{
	return GLEnumToString(filter);
}

char* Texture::GLEnumToString(GLenum gl_enum) const
{
	switch (gl_enum)
	{
	case GL_REPEAT:
		return "GL_REPEAT";
		break;
	case GL_LINEAR:
		return "GL_LINEAR";
		break;
	default:
		return "Not implemented yet";
		break;
	}
}
