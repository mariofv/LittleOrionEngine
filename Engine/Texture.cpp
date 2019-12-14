#include "Texture.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

Texture::Texture(unsigned char * data, int width, int height, const char* path) : data(data), width(width), height(height), texture_path(path)
{
	InitTexture();
}


Texture::~Texture()
{

	glDeleteTextures(1, &opengl_texture);
}

void Texture::InitTexture()
{
	glCreateTextures(GL_TEXTURE_2D, 1, &opengl_texture);
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

	

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_2D, 0);
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

void Texture::SetWrapS(const GLenum wrap_s)
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

void Texture::SetWrapT(const GLenum wrap_t)
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

void Texture::SetMinFilter(const GLenum min_filter)
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

void Texture::SetMagFilter(const GLenum mag_filter)
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

char* Texture::GLEnumToString(const GLenum gl_enum) const
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