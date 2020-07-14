#ifndef _PANELMATERIAL_H_
#define _PANELMATERIAL_H_

#include "ResourceManagement/Resources/Material.h"
#include "EditorUI/Panel/Panel.h"

class PanelMaterial : public Panel
{
public:
	PanelMaterial();
	~PanelMaterial() = default;
	
	void Render(std::shared_ptr<Material> material);
	bool IsMaterialExtracted(const std::shared_ptr<Material> &material);
	void ShowMaterialTextureMap(std::shared_ptr<Material> material, Material::MaterialTextureType type);

private:
	bool modified_by_user = false;
};

#endif //_PANELGAMEOBJECT_H_

