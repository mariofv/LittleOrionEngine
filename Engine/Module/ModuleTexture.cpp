#include "ModuleTexture.h"
#include "Globals.h"
#include "Application.h"
#include "Texture.h"
#include "Component/ComponentMaterial.h"
#include "Importer/MaterialImporter.h"

#include <SDL/SDL.h>
#include <algorithm>
#include <memory>

// Called before render is available
bool ModuleTexture::Init()
{
	APP_LOG_SECTION("************ Module Texture Init ************");
	GenerateCheckerboardTexture();
	GenerateWhiteFallTexture();
	return true;
}

// Called before quitting
bool ModuleTexture::CleanUp()
{
	glDeleteTextures(1, &checkerboard_texture_id);
	glDeleteTextures(1, &whitefall_texture_id);
	for (auto& material : materials) 
	{
		delete material;
	}
	materials.clear();
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
	auto it = std::find(materials.begin(), materials.end(), material_to_remove);
	if (it != materials.end())
	{
		delete *it;
		materials.erase(it);
	}
}

std::shared_ptr<Texture> ModuleTexture::LoadTexture(const char* texture_path)
{
	std::string ol_texture;
	bool imported = App->material_importer->Import(texture_path, ol_texture);
	if (!imported)
	{
		return nullptr;
	}

	return App->material_importer->Load(ol_texture.c_str());
}

GLuint ModuleTexture::LoadCubemap(std::vector<std::string> faces_paths) const
{
	std::vector<std::string> faces_paths_dds;
	for (unsigned int i = 0; i < faces_paths.size(); i++)
	{
		std::string ol_texture;
		bool imported = App->material_importer->Import(faces_paths[i].c_str(), ol_texture);
		faces_paths_dds.push_back(ol_texture);
	}
	return static_cast<GLuint>(App->material_importer->LoadCubemap(faces_paths_dds));
}


void ModuleTexture::GenerateCheckerboardTexture() {
	static GLubyte check_image[generate_texture_height][generate_texture_width][3];
	int i, j, color;
	for (i = 0; i < generate_texture_height; i++) {
		for (j = 0; j < generate_texture_width; j++) {
			color = ((((i & 0x4) == 0) ^ ((j & 0x4)) == 0)) * 255; // 0 -> black or 255 white
			check_image[i][j][0] = (GLubyte)color; //R
			check_image[i][j][1] = (GLubyte)color; //G
			check_image[i][j][2] = (GLubyte)color; //B
		}
	}
	GenerateTexture(checkerboard_texture_id, check_image);
}

void ModuleTexture::GenerateWhiteFallTexture() {


	static GLubyte white_image[generate_texture_height][generate_texture_width][3];
	int i, j, color;
	for (i = 0; i < generate_texture_height; i++) {
		for (j = 0; j < generate_texture_width; j++) {
			color = 255; // 0 -> black or 255 white
			white_image[i][j][0] = (GLubyte)color; //R
			white_image[i][j][1] = (GLubyte)color; //G
			white_image[i][j][2] = (GLubyte)color; //B
		}
	}
	GenerateTexture(whitefall_texture_id, white_image);
}

void ModuleTexture::GenerateTexture(GLuint &texture_id_to_store, GLubyte image[generate_texture_height][generate_texture_width][3])
{
	glGenTextures(1, &texture_id_to_store);
	glBindTexture(GL_TEXTURE_2D, texture_id_to_store);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, generate_texture_height, generate_texture_width, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

