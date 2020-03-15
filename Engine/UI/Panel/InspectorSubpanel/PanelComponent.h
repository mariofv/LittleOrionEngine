#ifndef _PANELCOMPONENT_H_
#define _PANELCOMPONENT_H_

#define ENGINE_EXPORTS

#include "Module/ModuleActions.h"
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

	void ShowScriptsCreated(ComponentScript*);

	void CheckClickedCamera(ComponentCamera* camera);
	void CheckClickForUndo(ModuleActions::UndoActionType type, Component* component);

	ENGINE_API void DropGOTarget(GameObject *& go, const std::string & script_name);

private:
	void DropTarget(ComponentMaterial *material, Texture::TextureType type);
	std::string GetTypeName(Texture::TextureType type);
};

#endif //_PANELCOMPONENT_H_

