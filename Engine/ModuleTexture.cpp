#include "Globals.h"
#include "Application.h"
#include "ModuleTexture.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include "SDL.h"

ModuleTexture::ModuleTexture()
{

}

// Destructor
ModuleTexture::~ModuleTexture()
{
}

// Called before render is available
bool ModuleTexture::Init()
{
	ilInit();
	iluInit();
	ilutInit();
	return true;
}

update_status ModuleTexture::PreUpdate()
{
	

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleTexture::Update()
{
	
	return UPDATE_CONTINUE;
}

update_status ModuleTexture::PostUpdate()
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleTexture::CleanUp()
{
	
	return true;
}

Texture* ModuleTexture::loadTexture(const char* texture_path) const
{
	ILuint image;
	ilGenImages(1, &image);

	ilBindImage(image);
	ilLoadImage(texture_path);

	ILenum error;
	error = ilGetError();
	if (error == IL_COULD_NOT_OPEN_FILE)
	{
		OPENGL_LOG_ERROR("Error loading texture %s. File not found", texture_path);
		return nullptr;
	}

	Texture *loaded_texture = new Texture(image, texture_path);
	loaded_texture->GenerateMipMap();

	return loaded_texture;
}


