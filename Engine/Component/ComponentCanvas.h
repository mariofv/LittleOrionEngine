#ifndef _COMPONENTCANVAS_H_
#define _COMPONENTCANVAS_H_

#include "ComponentUI.h"
#include "Component/ComponentCamera.h"
#include "MathGeoLib.h"
#include <GL/glew.h>

class ComponentCanvas : public Component
{
public:
	ComponentCanvas();
	ComponentCanvas(GameObject * owner);
	~ComponentCanvas() = default;

	ComponentCanvas & operator=(const ComponentCanvas& component_to_copy);
	ComponentCanvas & operator=(ComponentCanvas&& component_to_move) = default;

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;
	void Delete() override;

	void Render(float4x4* projection);

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;
	
};

#endif //_COMPONENTCANVAS_H_

