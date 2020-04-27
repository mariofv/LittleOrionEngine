#ifndef _PANELMATERIAL_H_
#define _PANELMATERIAL_H_

#include "ResourceManagement/Resources/Material.h"
#include "EditorUI/Panel/Panel.h"

class PanelMaterial : public Panel
{
public:
	PanelMaterial();
	~PanelMaterial() = default;
	
	void Render(Material* material);
	void ShowMaterialTextureMap(Material* material, Material::MaterialTextureType type);
	void SaveMaterial(Material* material);

private:
	void DropTarget(Material* material, Material::MaterialTextureType type);

	std::string GetTypeName(Material::MaterialTextureType type);

};

#endif //_PANELGAMEOBJECT_H_

