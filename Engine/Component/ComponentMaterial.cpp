#include "ComponentMaterial.h"
#include "Texture.h"
#include "Application.h"
#include <Module/ModuleTexture.h>
#include <Importer/MaterialImporter.h>

ComponentMaterial::ComponentMaterial() : Component(nullptr, ComponentType::MATERIAL)
{

}

ComponentMaterial::ComponentMaterial(GameObject * owner) : Component(owner, ComponentType::MATERIAL)
{

}

ComponentMaterial::~ComponentMaterial()
{
	App->material_importer->RemoveTextureFromCacheIfNeeded(texture);
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
