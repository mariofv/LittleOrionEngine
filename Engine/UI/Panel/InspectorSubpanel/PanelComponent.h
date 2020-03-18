#ifndef _PANELCOMPONENT_H_
#define _PANELCOMPONENT_H_

#include "Module/ModuleActions.h"
#include "ResourceManagement/Resources/Texture.h"

class Component;
class ComponentCanvas;
class ComponentCamera;
class ComponentLight;
class ComponentMaterial;
class ComponentMesh;
class ComponentTransform;
class ComponentUI;

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
	void ShowComponentCanvasWindow(ComponentCanvas* canvas);
	void ShowComponentUIWindow(ComponentUI*);

	void ShowAddNewComponentButton();

	void CheckClickedCamera(ComponentCamera* camera);
	void CheckClickForUndo(ModuleActions::UndoActionType type, Component* component);

	//Crear funcion de renderizar ui de inspector del transform 2d

private:
	void DropTarget(ComponentMaterial *material, Texture::TextureType type);
	std::string GetTypeName(Texture::TextureType type);
};

#endif //_PANELCOMPONENT_H_

