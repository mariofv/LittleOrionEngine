#ifndef _PANELCOMPONENT_H_
#define _PANELCOMPONENT_H_

#define ENGINE_EXPORTS

#include "Module/ModuleActions.h"

class Component;
class ComponentAnimation;
class ComponentButton;
class ComponentCamera;
class ComponentCanvas;
class ComponentImage;
class ComponentMeshRenderer;
class ComponentLight;
class ComponentProgressBar;
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
	bool ShowCommonUIWindow(ComponentUI* ui);
	void ShowComponentCanvasWindow(ComponentCanvas* canvas);
	void ShowComponentProgressBarWindow(ComponentProgressBar* progress_bar);
	void ShowComponentImageWindow(ComponentImage* image);
	void ShowComponentTextWindow(ComponentText* text);
	void ShowComponentButtonWindow(ComponentButton* button);
	void ShowAddNewComponentButton();

	void ShowScriptsCreated(ComponentScript* component_script);

	void CheckClickedCamera(ComponentCamera* camera);
	void CheckClickForUndo(ModuleActions::UndoActionType type, Component* component);

	ENGINE_API void DropGOTarget(GameObject*& go);

};

#endif //_PANELCOMPONENT_H_

