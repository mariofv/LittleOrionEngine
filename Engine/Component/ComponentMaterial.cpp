#include "ComponentMaterial.h"
#include "Texture.h"

ComponentMaterial::~ComponentMaterial()
{
	delete texture;
}

void ComponentMaterial::Enable()
{

}

void ComponentMaterial::Disable()
{

}

void ComponentMaterial::Update()
{

}

Component::ComponentType ComponentMaterial::GetType() const
{
	return Component::ComponentType::MATERIAL;
}