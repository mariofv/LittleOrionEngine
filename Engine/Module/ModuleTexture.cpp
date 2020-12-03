#include "ModuleTexture.h"

#include "Filesystem/Path.h"
#include "Log/EngineLog.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Main/Globals.h"
#include "Module/ModuleResourceManager.h"

#include "ResourceManagement/Importer/TextureImporter.h"
#include "ResourceManagement/Resources/Texture.h"

#include <algorithm>
#include <memory>
#include <SDL/SDL.h>

// Called before render is available
bool ModuleTexture::Init()
{
	APP_LOG_SECTION("************ Module Texture Init ************");
	GenerateTexture(checkerboard_texture_id, PATRON::CHECKBOARD);
	GenerateTexture(whitefall_texture_id, PATRON::WHITE);
	GenerateTexture(blackfall_texture_id, PATRON::BLACK);
	return true;
}

// Called before quitting
bool ModuleTexture::CleanUp()
{
	glDeleteTextures(1, &checkerboard_texture_id);
	glDeleteTextures(1, &whitefall_texture_id);
	glDeleteTextures(1, &blackfall_texture_id);
	return true;
}

GLubyte ModuleTexture::GetColor(size_t i, size_t j, PATRON color_id) const
{
	switch (color_id)
	{
	case PATRON::CHECKBOARD:
		return ((((i & 0x4) == 0) ^ ((j & 0x4)) == 0)) * 255;
		break;
	case PATRON::WHITE:
		return 255;
		break;
	case PATRON::BLACK:
		return 0;
		break;
	default:
		return 0;
		break;
	}

	
}

void ModuleTexture::GenerateTexture(GLuint &texture_id_to_store, PATRON color_id)
{

	GLubyte color = GetColor(0, 0, color_id);
	GLubyte texData[] = { color, color, color, color };

	glGenTextures(1, &texture_id_to_store);
	glBindTexture(GL_TEXTURE_2D, texture_id_to_store);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glBindTexture(GL_TEXTURE_2D, 0);
}

