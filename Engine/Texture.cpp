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

Texture::Texture(ILuint image)
{
	this->image = image;
	ILinfo ImageInfo;
	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}

	data = (unsigned char*)ilGetData();
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
}


Texture::~Texture()
{
	ilDeleteImages(1, &image);
}
