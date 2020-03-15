#include "ModuleTexture.h"
#include "Main/Globals.h"
#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Resources/Texture.h"
#include "Main/GameObject.h"
#include <Filesystem/File.h>
#include <ResourceManagement/Importer/TextureImporter.h>
#include <ResourceManagement/Loaders/TextureLoader.h>
#include <SDL/SDL.h>
#include <algorithm>
#include <memory>

// Called before render is available
bool ModuleTexture::Init()
{
	APP_LOG_SECTION("************ Module Texture Init ************");
	GenerateTexture(checkerboard_texture_id, PATRON::CHECKBOARD);
	GenerateTexture(whitefall_texture_id, PATRON::WHITE);
	return true;
}

// Called before quitting
bool ModuleTexture::CleanUp()
{
	glDeleteTextures(1, &checkerboard_texture_id);
	glDeleteTextures(1, &whitefall_texture_id);
	return true;
}

std::shared_ptr<Texture> ModuleTexture::LoadTexture(const char* texture_path)
{
	ImportResult import_result = App->resources->Import(File(texture_path));
	if (!import_result.succes)
	{
		return nullptr;
	}
	return App->resources->Load<Texture>(import_result.exported_file);
}

GLuint ModuleTexture::LoadCubemap(const std::vector<std::string> & faces_paths) const
{
	std::vector<std::string> faces_paths_dds;
	for (unsigned int i = 0; i < faces_paths.size(); i++)
	{
		std::string ol_texture = App->resources->Import(File(faces_paths[i]), true).exported_file;
		faces_paths_dds.push_back(ol_texture);
	}
	return static_cast<GLuint>(TextureLoader::LoadCubemap(faces_paths_dds));
}

GLubyte ModuleTexture::GetColor(size_t i, size_t j, PATRON color_id) const{


	switch (color_id)
	{
	case PATRON::CHECKBOARD:
		return ((((i & 0x4) == 0) ^ ((j & 0x4)) == 0)) * 255;
		break;
	case PATRON::WHITE:
		return 255;
		break;
	default:
		return 0;
		break;
	}
}

void ModuleTexture::GenerateTexture(GLuint &texture_id_to_store, PATRON color_id)
{
	static GLubyte image[generate_texture_height][generate_texture_width][3];
	int i, j, color;
	for (i = 0; i < generate_texture_height; i++) {
		for (j = 0; j < generate_texture_width; j++) {
			color = GetColor(i,j,color_id),
			image[i][j][0] = (GLubyte)color; //R
			image[i][j][1] = (GLubyte)color; //G
			image[i][j][2] = (GLubyte)color; //B
		}
	}
	glGenTextures(1, &texture_id_to_store);
	glBindTexture(GL_TEXTURE_2D, texture_id_to_store);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, generate_texture_height, generate_texture_width, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

