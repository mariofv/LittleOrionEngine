#ifndef _COMPONENTSUI_H_
#define _COMPONENTSUI_H_

class ComponentTransform;

class ComponentsUI
{
public:
	static void ShowComponentTransformWindow(ComponentTransform *transform);

private:
	ComponentsUI() = default;
	~ComponentsUI() = default;

};

#endif //_COMPONENTSUI_H_

