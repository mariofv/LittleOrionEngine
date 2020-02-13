#ifndef _COMPONENTSUI_H_
#define _COMPONENTSUI_H_

#include "Texture.h"
class ComponentCamera;
class ComponentMaterial;
class ComponentMesh;
class ComponentTransform;
class ComponentLight;
class Component;

class ComponentsUI
{
public:
	static void ShowComponentTransformWindow(ComponentTransform *transform);
	static void ShowComponentMeshWindow(ComponentMesh *mesh);
	static void ShowComponentMaterialWindow(ComponentMaterial *material);
	static void ShowComponentCameraWindow(ComponentCamera *camera);
	static void ShowComponentLightWindow(ComponentLight *light);

private:
	static void DropTarget(ComponentMaterial *material, Texture::TextureType type);
	static std::string GetTypeName(Texture::TextureType type);

	static void CheckClickedCamera(ComponentCamera* camera);

	static void CheckClickForUndo(const int type, Component* component);
	

	ComponentsUI() = default;
	~ComponentsUI() = default;

};

#endif //_COMPONENTSUI_H_

