#ifndef _PANELCOMPONENT_H_
#define _PANELCOMPONENT_H_

#include "Module/ModuleActions.h"
#include "ResourceManagement/Resources/Texture.h"

class Component;
class ComponentCamera;
class ComponentMaterial;
class ComponentMesh;
class ComponentTransform;
class ComponentLight;

class PanelComponent
{
public:
	PanelComponent() = default;
	~PanelComponent() = default;

	void ShowComponentTransformWindow(ComponentTransform *transform);
	void ShowComponentMeshWindow(ComponentMesh *mesh);
	void ShowComponentMaterialWindow(ComponentMaterial *material);
	void ShowComponentCameraWindow(ComponentCamera *camera);
	void ShowComponentLightWindow(ComponentLight *light);
	
	void ShowAddNewComponentButton();

	void CheckClickedCamera(ComponentCamera* camera);
	void CheckClickForUndo(ModuleActions::UndoActionType type, Component* component);

private:
	void DropTarget(ComponentMaterial *material, Texture::TextureType type);
	std::string GetTypeName(Texture::TextureType type);
};

#endif //_PANELCOMPONENT_H_

