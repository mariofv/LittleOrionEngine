#include "ModuleTexture.h"
#include "Globals.h"
#include "Application.h"
#include "Texture.h"
#include "Component/ComponentMaterial.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <SDL/SDL.h>

// Called before render is available
bool ModuleTexture::Init()
{
	APP_LOG_SECTION("************ Module Texture Init ************");

	APP_LOG_INIT("Initializing DevIL image loader.")
	ilInit();
	iluInit();
	ilutInit();
	APP_LOG_SUCCESS("DevIL image loader initialized correctly.")

	return true;
}

// Called before quitting
bool ModuleTexture::CleanUp()
{
	
	return true;
}

ComponentMaterial* ModuleTexture::CreateComponentMaterial() const
{
	return new ComponentMaterial();
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


