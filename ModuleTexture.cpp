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

void ModuleTexture::loadTexture(const char* texture_path)
{
	ILuint image;
	ilGenImages(1, &image);

	ilBindImage(image);
	ilLoadImage(texture_path);

	ILenum error;
	error = ilGetError();

	texture_data = (unsigned char*)ilGetData();
	texture_width = ilGetInteger(IL_IMAGE_WIDTH);
	texture_height = ilGetInteger(IL_IMAGE_HEIGHT);

	return;
}


