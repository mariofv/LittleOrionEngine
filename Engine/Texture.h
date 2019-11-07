#pragma once
#include "Module.h"
#include "Globals.h"
#include <IL/il.h>


class Texture
{
public:
	Texture();
	Texture(ILuint image);

	~Texture();

public:
	ILuint image;

	const unsigned char* data = nullptr;
	int width;
	int height;

};
