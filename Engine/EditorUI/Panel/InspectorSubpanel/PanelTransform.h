#ifndef _PANELTRANSFORM_H_
#define _PANELTRANSFORM_H_

#define ENGINE_EXPORTS

#include "Module/ModuleActions.h"

class Component;
class ComponentTransform;
class ComponentTransform2D;

class PanelTransform
{
public:
	PanelTransform() = default;
	~PanelTransform() = default;

	static void ShowComponentTransformWindow(ComponentTransform* transform);
	static void ShowComponentTransform2DWindow(ComponentTransform2D *transform_2D);

	static void CheckClickForUndo(ModuleActions::UndoActionType type, Component* component);
};

#endif //_PANELTRANSFORM_H_

