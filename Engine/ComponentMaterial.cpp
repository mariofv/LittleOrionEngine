#include "ComponentMaterial.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

ComponentMaterial::ComponentMaterial(ILuint image, const char* path)
{
	this->image = image;
	this->material_texture_path = path;

	ILinfo ImageInfo;
	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}

	data = (unsigned char*)ilGetData();
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);

	InitMaterialTexture();
}


ComponentMaterial::~ComponentMaterial()
{
	ilDeleteImages(1, &image);
	glDeleteTextures(1, &opengl_texture);
}

void ComponentMaterial::InitMaterialTexture()
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



	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void ComponentMaterial::GenerateMipMap()
{
	mip_map = true;
	glBindTexture(GL_TEXTURE_2D, opengl_texture);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool ComponentMaterial::IsMipMapped() const
{
	return mip_map;
}

void ComponentMaterial::SetWrapS(const GLenum wrap_s)
{
	this->wrap_s = wrap_s;
	glBindTexture(GL_TEXTURE_2D, opengl_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLenum ComponentMaterial::GetWrapS() const
{
	return wrap_s;
}

char* ComponentMaterial::GetWrapS_C_Str() const
{
	return GLEnumToString(wrap_s);
}

void ComponentMaterial::SetWrapT(const GLenum wrap_t)
{
	this->wrap_t = wrap_t;
	glBindTexture(GL_TEXTURE_2D, opengl_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLenum ComponentMaterial::GetWrapT() const
{
	return wrap_t;
}

char* ComponentMaterial::GetWrapT_C_Str() const
{
	return GLEnumToString(wrap_t);
}

void ComponentMaterial::SetMinFilter(const GLenum min_filter)
{
	this->min_filter = min_filter;
	glBindTexture(GL_TEXTURE_2D, opengl_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLenum ComponentMaterial::GetMinFilter() const
{
	return min_filter;
}

char* ComponentMaterial::GetMinFilter_C_Str() const
{
	return GLEnumToString(min_filter);
}

void ComponentMaterial::SetMagFilter(const GLenum mag_filter)
{
	this->mag_filter = mag_filter;
	glBindTexture(GL_TEXTURE_2D, opengl_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLenum ComponentMaterial::GetMagFilter() const
{
	return mag_filter;
}

char* ComponentMaterial::GetMagFilter_C_Str() const
{
	return GLEnumToString(mag_filter);
}

char* ComponentMaterial::GLEnumToString(const GLenum gl_enum) const
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