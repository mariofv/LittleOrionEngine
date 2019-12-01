#ifndef _COMPONENTSUI_H_
#define _COMPONENTSUI_H_

class ComponentMaterial;
class ComponentMesh;
class ComponentTransform;

class ComponentsUI
{
public:
	static void ShowComponentTransformWindow(ComponentTransform *transform);
	static void ShowComponentMeshWindow(ComponentMesh *mesh);
	static void ShowComponentMaterialWindow(ComponentMaterial *material);


private:
	ComponentsUI() = default;
	~ComponentsUI() = default;

};

#endif //_COMPONENTSUI_H_

