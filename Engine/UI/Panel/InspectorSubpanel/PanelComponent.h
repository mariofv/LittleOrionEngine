#ifndef _PANELCOMPONENT_H_
#define _PANELCOMPONENT_H_

#include "Module/ModuleEditor.h"
#include "ResourceManagement/Resources/Texture.h"

class Component;
class ComponentCamera;
class ComponentMaterial;
class ComponentMesh;
class ComponentTransform;
class ComponentLight;
class ComponentScript;

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
	void ShowComponentScriptWindow(ComponentScript * component_script);
	
	void ShowAddNewComponentButton();

	void ShowScriptsCreated();

	void CheckClickedCamera(ComponentCamera* camera);
	void CheckClickForUndo(ModuleEditor::UndoActionType type, Component* component);

private:
	void DropTarget(ComponentMaterial *material, Texture::TextureType type);
	std::string GetTypeName(Texture::TextureType type);
};

#endif //_PANELCOMPONENT_H_

