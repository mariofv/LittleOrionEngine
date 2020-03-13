#ifndef _PANELCOMPONENT_H_
#define _PANELCOMPONENT_H_

#include "Module/ModuleActions.h"

class Component;
class ComponentCamera;
class ComponentMeshRenderer;
class ComponentTransform;
class ComponentLight;
class ComponentScript;

class PanelComponent
{
public:
	PanelComponent() = default;
	~PanelComponent() = default;

	void ShowComponentTransformWindow(ComponentTransform *transform);
	void ShowComponentMeshRendererWindow(ComponentMeshRenderer *mesh);
	void ShowComponentCameraWindow(ComponentCamera *camera);
	void ShowComponentLightWindow(ComponentLight *light);
	void ShowComponentScriptWindow(ComponentScript * component_script);
	
	void ShowAddNewComponentButton();

	void ShowScriptsCreated(ComponentScript*);

	void CheckClickedCamera(ComponentCamera* camera);
	void CheckClickForUndo(ModuleActions::UndoActionType type, Component* component);

private:
	void DropMeshAndMaterial(ComponentMeshRenderer* component_mesh);
};

#endif //_PANELCOMPONENT_H_

