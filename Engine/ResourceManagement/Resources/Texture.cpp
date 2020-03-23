#include "Texture.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>


Texture::Texture(char * data, size_t image_size, int width, int height, const std::string& path, bool normal_map,TextureType type) : image_size(image_size), width(width), height(height), normal_map(normal_map),type(type), data(data), Resource("",path)
{
	LoadInMemory();
}


Texture::~Texture()
{

	glDeleteTextures(1, &opengl_texture);
}

void Texture::LoadInMemory()
{
	glGenTextures(1, &opengl_texture);
	glBindTexture(GL_TEXTURE_2D, opengl_texture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	wrap_s = GL_REPEAT;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);

	wrap_t = GL_REPEAT;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);

	min_filter = GL_LINEAR;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);

	mag_filter = GL_LINEAR;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

	if (normal_map)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else 
	{
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, width, height, 0, image_size, data);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	GenerateMipMap();
}

void Texture::GenerateMipMap()
{
	mip_map = true;
	glBindTexture(GL_TEXTURE_2D, opengl_texture);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool Texture::IsMipMapped() const
{
	return mip_map;
}

void Texture::SetWrapS(GLenum wrap_s)
{
	this->wrap_s = wrap_s;
	glBindTexture(GL_TEXTURE_2D, opengl_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLenum Texture::GetWrapS() const
{
	return wrap_s;
}

char* Texture::GetWrapS_C_Str() const
{
	return GLEnumToString(wrap_s);
}

void Texture::SetWrapT(GLenum wrap_t)
{
	this->wrap_t = wrap_t;
	glBindTexture(GL_TEXTURE_2D, opengl_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLenum Texture::GetWrapT() const
{
	return wrap_t;
}

char* Texture::GetWrapT_C_Str() const
{
	return GLEnumToString(wrap_t);
}

void Texture::SetMinFilter(GLenum min_filter)
{
	this->min_filter = min_filter;
	glBindTexture(GL_TEXTURE_2D, opengl_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLenum Texture::GetMinFilter() const
{
	return min_filter;
}

char* Texture::GetMinFilter_C_Str() const
{
	return GLEnumToString(min_filter);
}

void Texture::SetMagFilter(GLenum mag_filter)
{
	this->mag_filter = mag_filter;
	glBindTexture(GL_TEXTURE_2D, opengl_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLenum Texture::GetMagFilter() const
{
	return mag_filter;
}

char* Texture::GetMagFilter_C_Str() const
{
	return GLEnumToString(mag_filter);
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
