#include "Globals.h"
#include "Application.h"
#include "ModuleTexture.h"
#include <GL/glew.h>
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

const Texture* ModuleTexture::loadTexture(const char* texture_path) const
{
	ILuint image;
	ilGenImages(1, &image);

	ilBindImage(image);
	ilLoadImage(texture_path);

	ILenum error;
	error = ilGetError();

	Texture *loaded_texture = new Texture(image);

	return loaded_texture;
}


