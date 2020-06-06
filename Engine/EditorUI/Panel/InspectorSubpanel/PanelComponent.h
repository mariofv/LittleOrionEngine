#ifndef _PANELCOMPONENT_H_
#define _PANELCOMPONENT_H_

#define ENGINE_EXPORTS

#include "Module/ModuleActions.h"

class Component;
class ComponentAnimation;
class ComponentAudioSource;
class ComponentBillboard;
class ComponentBoxCollider;
class ComponentButton;
class ComponentCamera;
class ComponentCanvas;
class ComponentCanvasRenderer;
class ComponentCapsuleCollider;
class ComponentCollider;
class ComponentCylinderCollider;
class ComponentEventSystem;
class ComponentImage;
class ComponentMeshCollider;
class ComponentMeshRenderer;
class ComponentTransform;
class ComponentLight;
class ComponentParticleSystem;
class ComponentSphereCollider;
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
	void ShowComponentMeshRendererWindow(ComponentMeshRenderer* mesh_renderer);
	void ShowComponentCanvasRendererWindow(ComponentCanvasRenderer* canvas_renderer);
	void ShowComponentBillboard(ComponentBillboard* billboard);

	void ShowComponentCameraWindow(ComponentCamera* camera);
	void ShowComponentLightWindow(ComponentLight* light);
	void ShowComponentAnimationWindow(ComponentAnimation* animation);
	void ShowComponentScriptWindow(ComponentScript* component_script);

	void ShowComponentCanvasWindow(ComponentCanvas* canvas);
	void ShowComponentImageWindow(ComponentImage* image);
	void ShowComponentTextWindow(ComponentText* text);
	void ShowComponentButtonWindow(ComponentButton* button);

	void ShowComponentEventSystem(ComponentEventSystem* event_system);

	void ShowComponentColliderWindow(ComponentCollider* collider);
	void ShowComponentAudioSourceWindow(ComponentAudioSource* component_audio_source);
	void ShowComponentParticleSystem(ComponentParticleSystem* particle_system);

	void ShowAddNewComponentButton();
	bool ShowCommonComponentWindow(Component* component);

	void ShowScriptsCreated(ComponentScript* component_script);

	void CheckClickedCamera(ComponentCamera* camera);
	void CheckClickForUndo(ModuleActions::UndoActionType type, Component* component);

	ENGINE_API void DropGOTarget(GameObject*& go);

private:
	bool ShowCommonColliderWindow(ComponentCollider* collider);
	void ShowComponentBoxColliderWindow(ComponentBoxCollider* box_collider);
	void ShowComponentCapsuleColliderWindow(ComponentCapsuleCollider* capsule_collider);
	void ShowComponentSphereColliderWindow(ComponentSphereCollider* sphere_collider);
	void ShowComponentCylinderColliderWindow(ComponentCylinderCollider* cylinder_collider);
	void ShowComponentMeshColliderWindow(ComponentMeshCollider* mesh_collider);
};

#endif //_PANELCOMPONENT_H_
