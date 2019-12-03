#include "ModuleTexture.h"
#include "Globals.h"
#include "Application.h"
#include "Texture.h"
#include "Component/ComponentMaterial.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <SDL/SDL.h>
#include <algorithm>

// Called before render is available
bool ModuleTexture::Init()
{
	APP_LOG_SECTION("************ Module Texture Init ************");

	APP_LOG_INIT("Initializing DevIL image loader.")
	ilInit();
	iluInit();
	ilutInit();
	GenerateCheckerboardTexture();
	APP_LOG_SUCCESS("DevIL image loader initialized correctly.")

	return true;
}

// Called before quitting
bool ModuleTexture::CleanUp()
{
	glDeleteTextures(1, &checkerboard_texture_id);
	return true;
}

ComponentMaterial* ModuleTexture::CreateComponentMaterial()
{
	ComponentMaterial * new_material = new ComponentMaterial();
	materials.push_back(new_material);
	return new_material;
}

void ModuleTexture::RemoveComponentMaterial(ComponentMaterial* material_to_remove)
{
	auto it = std::remove_if(materials.begin(), materials.end(), [material_to_remove](auto const & material)
	{
		return material == material_to_remove;
	});
	materials.erase(it);
}

Texture* ModuleTexture::LoadTexture(const char* texture_path) const
{
	ILuint image;
	ilGenImages(1, &image);

	ilBindImage(image);
	ilLoadImage(texture_path);

	ILenum error;
	error = ilGetError();
	if (error == IL_COULD_NOT_OPEN_FILE)
	{
		APP_LOG_ERROR("Error loading texture %s. File not found", texture_path);
		return nullptr;
	}

	ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

	Texture *loaded_texture = new Texture(image, texture_path);
	loaded_texture->GenerateMipMap();

	return loaded_texture;
}

void ModuleTexture::GenerateCheckerboardTexture() {
	const static int checkerHeight = 64;
	const static int checkWidth = 64;

	static GLubyte checkImage[checkerHeight][checkWidth][3];
	int i, j, color;
	for (i = 0; i < checkerHeight; i++) {
		for (j = 0; j < checkWidth; j++) {
			color = ((((i & 0x4) == 0) ^ ((j & 0x4)) == 0)) * 255; // 0 -> black or 255 white
			checkImage[i][j][0] = (GLubyte)color; //R
			checkImage[i][j][1] = (GLubyte)color; //G
			checkImage[i][j][2] = (GLubyte)color; //B
		}
	}

	glGenTextures(1, &checkerboard_texture_id);
	glBindTexture(GL_TEXTURE_2D, checkerboard_texture_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, checkerHeight, checkWidth, 0, GL_RGB, GL_UNSIGNED_BYTE, checkImage);
	glBindTexture(GL_TEXTURE_2D, 0);
}


