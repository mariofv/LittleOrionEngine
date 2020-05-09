#ifndef _PANELCOMPONENT_H_
#define _PANELCOMPONENT_H_

#define ENGINE_EXPORTS

#include "Module/ModuleActions.h"

class Component;
class ComponentAnimation;
class ComponentBoxCollider;
class ComponentButton;
class ComponentCamera;
class ComponentCanvas;
class ComponentCapsuleCollider;
class ComponentCollider;
class ComponentCylinderCollider;
class ComponentImage;
class ComponentMeshCollider;
class ComponentMeshRenderer;
class ComponentLight;
class ComponentProgressBar;
class ComponentSphereCollider;
class ComponentUI;
class ComponentText;
class ComponentTransform;
class ComponentScript;
class GameObject;


class PanelComponent
{
public:
	PanelComponent() = default;
	~PanelComponent() = default;
	
	void ShowComponentTransformWindow(ComponentTransform* transform);
	void ShowComponentMeshRendererWindow(ComponentMeshRenderer* mesh);
	void ShowComponentCameraWindow(ComponentCamera* camera);
	void ShowComponentLightWindow(ComponentLight* light);
	void ShowComponentAnimationWindow(ComponentAnimation* animation);
	void ShowComponentScriptWindow(ComponentScript* component_script);
	void ShowComponentUIWindow(ComponentUI* ui);
	void ShowComponentColliderWindow(ComponentCollider* collider);
	void ShowAddNewComponentButton();

	void ShowScriptsCreated(ComponentScript* component_script);

	void CheckClickedCamera(ComponentCamera* camera);
	void CheckClickForUndo(ModuleActions::UndoActionType type, Component* component);

	ENGINE_API void DropGOTarget(GameObject*& go);

private:
	void ShowCommonUIWindow(ComponentUI* ui);
	void ShowComponentCanvasWindow(ComponentCanvas* canvas);
	void ShowComponentProgressBarWindow(ComponentProgressBar* progress_bar);
	void ShowComponentImageWindow(ComponentImage* image);
	void ShowComponentTextWindow(ComponentText* text);
	void ShowComponentButtonWindow(ComponentButton* button);

	bool ShowCommonColliderWindow(ComponentCollider* collider);
	void ShowComponentBoxColliderWindow(ComponentBoxCollider* box_collider);
	void ShowComponentCapsuleColliderWindow(ComponentCapsuleCollider* capsule_collider);
	void ShowComponentSphereColliderWindow(ComponentSphereCollider* sphere_collider);
	void ShowComponentCylinderColliderWindow(ComponentCylinderCollider* cylinder_collider);
	void ShowComponentMeshColliderWindow(ComponentMeshCollider* mesh_collider);
};

#endif //_PANELCOMPONENT_H_

