#ifndef _PANELCOMPONENT_H_
#define _PANELCOMPONENT_H_

#define ENGINE_EXPORTS

#include "Module/ModuleActions.h"

class Component;
class ComponentAnimation;
class ComponentCamera;
class ComponentMeshRenderer;
class ComponentTransform;
class ComponentLight;
class ComponentScript;
class GameObject;

class PanelComponent
{
public:
	PanelComponent() = default;
	~PanelComponent() = default;

	void ShowComponentTransformWindow(ComponentTransform *transform);
	void ShowComponentMeshRendererWindow(ComponentMeshRenderer *mesh);
	void ShowComponentCameraWindow(ComponentCamera *camera);
	void ShowComponentLightWindow(ComponentLight *light);
	void ShowComponentAnimationWindow(ComponentAnimation *animation);
	void ShowComponentScriptWindow(ComponentScript * component_script);
	
	void ShowAddNewComponentButton();

	void ShowScriptsCreated(ComponentScript*);

	void CheckClickedCamera(ComponentCamera* camera);
	void CheckClickForUndo(ModuleActions::UndoActionType type, Component* component);

	ENGINE_API void DropGOTarget(GameObject *& go, const std::string & script_name, ComponentScript *& script_to_find);

private:
	void DropMeshAndMaterial(ComponentMeshRenderer* component_mesh);
	void DropAnimationAndSkeleton(ComponentAnimation* component_animation);
};

#endif //_PANELCOMPONENT_H_

