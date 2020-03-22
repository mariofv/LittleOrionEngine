#ifndef _PANELCOMPONENT_H_
#define _PANELCOMPONENT_H_

#define ENGINE_EXPORTS

#include "Module/ModuleActions.h"

class Component;
class ComponentCamera;
class ComponentCanvas;
class ComponentMeshRenderer;
class ComponentLight;
class ComponentScript;
class ComponentText;
class ComponentTransform;
class ComponentUI;

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
	void ShowComponentCanvasWindow(ComponentCanvas* canvas);
	void ShowComponentUIWindow(ComponentUI*);
	void ShowComponentTextWindow(ComponentText*);
	
	void ShowAddNewComponentButton();

	void ShowScriptsCreated(ComponentScript*);

	void CheckClickedCamera(ComponentCamera* camera);
	void CheckClickForUndo(ModuleActions::UndoActionType type, Component* component);

	ENGINE_API void DropGOTarget(GameObject *& go, const std::string & script_name, ComponentScript *& script_to_find);

private:
	void DropMeshAndMaterial(ComponentMeshRenderer* component_mesh);
};

#endif //_PANELCOMPONENT_H_

