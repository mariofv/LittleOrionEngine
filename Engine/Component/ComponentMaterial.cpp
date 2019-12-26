#include "ComponentMaterial.h"
#include "Application.h"
#include <Module/ModuleTexture.h>
#include <Importer/MaterialImporter.h>

ComponentMaterial::ComponentMaterial() : Component(nullptr, ComponentType::MATERIAL)
{
	textures.resize(Texture::MAX_TEXTURE_TYPES);
}

ComponentMaterial::ComponentMaterial(GameObject * owner) : Component(owner, ComponentType::MATERIAL)
{
	textures.resize(Texture::MAX_TEXTURE_TYPES);
}

ComponentMaterial::~ComponentMaterial()
{
	for (auto texture : textures)
	{
		App->material_importer->RemoveTextureFromCacheIfNeeded(texture);
	}
}


void ComponentMaterial::Delete()
{
	App->texture->RemoveComponentMaterial(this);
}

void ComponentMaterial::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddInt((unsigned int)type, "ComponentType");
	config.AddBool(active, "Active");
	config.AddInt(index, "Index");
	for (size_t i = 0; i< textures.size(); i++ )
	{
		if (textures[i] != nullptr) 
		{
			std::string id = "Path" + i;
			config.AddString(textures[i]->texture_path, id);
		}
	}
	config.AddBool(show_checkerboard_texture, "Checkboard");
}

void ComponentMaterial::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);

	index = config.GetInt("Index", 0);

	std::string tmp_path;
	config.GetString("Path", tmp_path, "");
	textures.resize(Texture::MAX_TEXTURE_TYPES);
	for (size_t i = 0; i < textures.size(); i++)
	{
		std::string id = "Path" + i;
		std::string tmp_path;
		config.GetString(id, tmp_path, "");
		if (!tmp_path.empty())
		{
			textures[i] = App->material_importer->Load(tmp_path.c_str());
		}
	}
	
	show_checkerboard_texture = config.GetBool("Checkboard", true);

}

void ComponentMaterial::Render(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE0);
	AddDiffuseUniform(shader_program);
	AddEmissiveUniform(shader_program);
	AddSpecularUniform(shader_program);
	AddAmbientOclusionUniform(shader_program);

}

void ComponentMaterial::AddDiffuseUniform(unsigned int shader_program) const
{
	bool use_diffuse_map = false;
	if (textures[Texture::TextureType::DIFUSSE] != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, textures[Texture::TextureType::DIFUSSE]->opengl_texture);
		glUniform1i(glGetUniformLocation(shader_program, "material.diffuse_map"), 0);
		use_diffuse_map = true;
	}
	glUniform1i(glGetUniformLocation(shader_program, "material.use_diffuse_map"), use_diffuse_map);
	glUniform3fv(glGetUniformLocation(shader_program, "material.diffuse_color"), sizeof(float), (float*)diffuse_color);
	glUniform1f(glGetUniformLocation(shader_program, "material.k_diffuse"),  k_diffuse);

}
void ComponentMaterial::AddEmissiveUniform(unsigned int shader_program) const
{
	bool use_emissive_map = false;
	if (textures[Texture::TextureType::EMISSIVE] != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, textures[Texture::TextureType::EMISSIVE]->opengl_texture);
		glUniform1i(glGetUniformLocation(shader_program, "material.emissive_map"), 0);
		use_emissive_map = true;
	}
	glUniform1i(glGetUniformLocation(shader_program, "material.use_emissive_map"), use_emissive_map);
	glUniform3fv(glGetUniformLocation(shader_program, "material.emissive_color"), sizeof(float), (float*)emissive_color);
}
void ComponentMaterial::AddSpecularUniform(unsigned int shader_program) const
{
	bool use_specular_map = false;
	if (textures[Texture::TextureType::SPECULAR] != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, textures[Texture::TextureType::SPECULAR]->opengl_texture);
		glUniform1i(glGetUniformLocation(shader_program, "material.specular_map"), 0);
		use_specular_map = true;
	}

	glUniform1i(glGetUniformLocation(shader_program, "material.use_specular_map"), use_specular_map);
	glUniform3fv(glGetUniformLocation(shader_program, "material.specular_color"), sizeof(float), (float*)specular_color);
	glUniform1f(glGetUniformLocation(shader_program, "material.k_specular"), k_specular);
	glUniform1f(glGetUniformLocation(shader_program, "material.shininess"), shininess);
}
void ComponentMaterial::AddAmbientOclusionUniform(unsigned int shader_program) const
{
	bool use_oclussion_map = false;
	if (textures[Texture::TextureType::OCLUSION] != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, textures[Texture::TextureType::OCLUSION]->opengl_texture);
		glUniform1i(glGetUniformLocation(shader_program, "material.occlusion_map"), 0);
		use_oclussion_map = true;
	}
	glUniform1i(glGetUniformLocation(shader_program, "material.use_oclussion_map"), use_oclussion_map);
	glUniform1f(glGetUniformLocation(shader_program, "material.k_ambient"), k_ambient);
}

void ComponentMaterial::SetMaterialTexture(Texture::TextureType type, std::shared_ptr<Texture> & new_texture)
{
	textures[static_cast<int>(type)] = new_texture;
}
const std::shared_ptr<Texture>& ComponentMaterial::GetMaterialTexture(Texture::TextureType type) const
{
	return textures[static_cast<int>(type)];
}

void ComponentMaterial::SetMaterialTexture(size_t type, std::shared_ptr<Texture> & new_texture)
{
	textures[type] = new_texture;
}
const std::shared_ptr<Texture>& ComponentMaterial::GetMaterialTexture(size_t  type) const
{
	return textures[type];
}
void ComponentMaterial::ShowComponentWindow()
{
	ComponentsUI::ShowComponentMaterialWindow(this);
}
