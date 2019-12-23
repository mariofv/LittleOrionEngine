#include "ComponentMaterial.h"
#include "Application.h"
#include <Module/ModuleTexture.h>
#include <Importer/MaterialImporter.h>

ComponentMaterial::ComponentMaterial() : Component(nullptr, ComponentType::MATERIAL)
{
	textures.resize(5);
}

ComponentMaterial::ComponentMaterial(GameObject * owner) : Component(owner, ComponentType::MATERIAL)
{
	textures.resize(5);
}

ComponentMaterial::~ComponentMaterial()
{
	for (auto texture : textures)
	{
		App->material_importer->RemoveTextureFromCacheIfNeeded(texture);
	}
}

void ComponentMaterial::Enable()
{
	active = true;
}

void ComponentMaterial::Disable()
{
	active = false;
}

void ComponentMaterial::Update()
{

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
	for (size_t i ; i< textures.size(); i++ )
	{
		std::string id = "Path" + i;
		config.AddString(textures[i]->texture_path, id);
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
	for (size_t i; i < textures.size(); i++)
	{
		std::string id = "Path" + i;
		std::string tmp_path;
		config.GetString(id, tmp_path, "");
		textures[i] = App->material_importer->Load(tmp_path.c_str());
	}
	
	show_checkerboard_texture = config.GetBool("Checkboard", true);

}

GLuint ComponentMaterial::GetTexture() const
{
	if (show_checkerboard_texture)
	{
		return App->texture->checkerboard_texture_id;
	}
	return active ? textures[1]->opengl_texture : 0;
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
