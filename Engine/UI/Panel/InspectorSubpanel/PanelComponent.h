#ifndef _PANELCOMPONENT_H_
#define _PANELCOMPONENT_H_

#include "Module/ModuleActions.h"

class Component;
class ComponentCamera;
class ComponentMaterialRenderer;
class ComponentMeshRenderer;
class ComponentTransform;
class ComponentLight;

class PanelComponent
{
public:
	PanelComponent() = default;
	~PanelComponent() = default;

	void ShowComponentTransformWindow(ComponentTransform *transform);
	void ShowComponentMeshRendererWindow(ComponentMeshRenderer *mesh);
	void ShowComponentCameraWindow(ComponentCamera *camera);
	void ShowComponentLightWindow(ComponentLight *light);
	
	void ShowAddNewComponentButton();

	void CheckClickedCamera(ComponentCamera* camera);
	void CheckClickForUndo(ModuleActions::UndoActionType type, Component* component);
};

#endif //_PANELCOMPONENT_H_

