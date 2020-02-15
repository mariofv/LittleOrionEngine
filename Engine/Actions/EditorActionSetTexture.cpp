#include "EditorActionSetTexture.h"



EditorActionSetTexture::EditorActionSetTexture(ComponentMaterial* comp, Texture::TextureType typo)
{
	component = comp;
	type = typo;
	texture = component->GetMaterialTexture(type);
}


EditorActionSetTexture::~EditorActionSetTexture()
{
}

void EditorActionSetTexture::Undo()
{
	std::shared_ptr<Texture> aux = component->GetMaterialTexture(type);
	component->SetMaterialTexture(type, texture);
	texture = aux;
}

void EditorActionSetTexture::Redo()
{
	std::shared_ptr<Texture> aux = component->GetMaterialTexture(type);
	component->SetMaterialTexture(type, texture);
	texture = aux;
}
