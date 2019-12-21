#include "ComponentMaterial.h"
#include "Texture.h"
#include "Application.h"
#include <Module/ModuleTexture.h>

ComponentMaterial::ComponentMaterial() : Component(nullptr, ComponentType::MATERIAL)
{

}

ComponentMaterial::ComponentMaterial(GameObject * owner) : Component(owner, ComponentType::MATERIAL)
{

}

ComponentMaterial::~ComponentMaterial()
{
	delete texture;
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
	config.AddString(texture->texture_path, "Path");
	config.AddBool(show_checkerboard_texture, "Checkboard");
}

void ComponentMaterial::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);

	index = config.GetInt("Index", 0);

	std::string tmp_path;
	config.GetString("Path", tmp_path, "");
	texture = App->texture->LoadTexture(tmp_path.c_str());
	
	show_checkerboard_texture = config.GetBool("Checkboard", true);
}

GLuint ComponentMaterial::GetTexture() const
{
	if (show_checkerboard_texture)
	{
		return App->texture->checkerboard_texture_id;
	}
	return active ? texture->opengl_texture : 0;
}

void ComponentMaterial::ShowComponentWindow()
{
	ComponentsUI::ShowComponentMaterialWindow(this);
}
