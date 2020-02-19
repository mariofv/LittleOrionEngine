#include "EditorActionSetTexture.h"
#include "Module/ModuleScene.h"
#include "Main/Application.h"


EditorActionSetTexture::EditorActionSetTexture(ComponentMaterial* comp, Texture::TextureType typo)
{
	UUID_COMP = comp->UUID;
	type = typo;
	texture = comp->GetMaterialTexture(type);
}

void EditorActionSetTexture::Undo()
{
	ComponentMaterial* component = (ComponentMaterial*)App->scene->GetComponent(UUID_COMP);
	std::shared_ptr<Texture> aux = component->GetMaterialTexture(type);
	component->SetMaterialTexture(type, texture);
	texture = aux;
}

void EditorActionSetTexture::Redo()
{
	ComponentMaterial* component = (ComponentMaterial*)App->scene->GetComponent(UUID_COMP);
	std::shared_ptr<Texture> aux = component->GetMaterialTexture(type);
	component->SetMaterialTexture(type, texture);
	texture = aux;
}
