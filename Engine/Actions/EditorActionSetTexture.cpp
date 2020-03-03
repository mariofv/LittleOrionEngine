#include "EditorActionSetTexture.h"
#include "Module/ModuleScene.h"
#include "Main/Application.h"


EditorActionSetTexture::EditorActionSetTexture(ComponentMaterialRenderer* comp, Material::MaterialTextureType typo) : component_UUID(comp->UUID),
type(typo), texture(comp->GetMaterialTexture(type)) {}

void EditorActionSetTexture::Undo()
{
	ComponentMaterialRenderer* component = (ComponentMaterialRenderer*)App->scene->GetComponent(component_UUID);
	std::shared_ptr<Texture> aux = component->GetMaterialTexture(type);
	component->SetMaterialTexture(type, texture);
	texture = aux;
}

void EditorActionSetTexture::Redo()
{
	ComponentMaterialRenderer* component = (ComponentMaterialRenderer*)App->scene->GetComponent(component_UUID);
	std::shared_ptr<Texture> aux = component->GetMaterialTexture(type);
	component->SetMaterialTexture(type, texture);
	texture = aux;
}
