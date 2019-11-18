#include "Texture.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>


Texture::Texture() :
	data(nullptr),
	width(0),
	height(0),
	image(0)
{}

Texture::Texture(ILuint image, const char* path)
{
	this->image = image;
	this->texture_path = path;

	ILinfo ImageInfo;
	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}

	data = (unsigned char*)ilGetData();
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);

	InitTexture();
}


Texture::~Texture()
{
	ilDeleteImages(1, &image);
	glDeleteTextures(1, &opengl_texture);
}

void Texture::InitTexture()
{
	glCreateTextures(GL_TEXTURE_2D, 1, &opengl_texture);
	glBindTexture(GL_TEXTURE_2D, opengl_texture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}