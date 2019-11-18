#pragma once
#include "Module.h"
#include "Globals.h"

#include <IL/il.h>
#include <GL/glew.h>


class Texture
{
public:
	Texture();
	Texture(ILuint image, const char* path);

	~Texture();

private:
	void InitTexture();

public:
	ILuint image;
	GLuint opengl_texture;

	const unsigned char* data = nullptr;
	const char *texture_path = nullptr;

	int width;
	int height;

};
