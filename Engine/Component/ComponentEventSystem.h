#ifndef _COMPONENTEVENTSYSTEM_H_
#define _COMPONENTEVENTSYSTEM_H_

#include "Component/Component.h"

#include <GL/glew.h>
#include <MathGeoLib.h>

class ComponentEventSystem: public Component
{
public:
	ComponentEventSystem();
	ComponentEventSystem(GameObject * owner);
	~ComponentEventSystem() = default;

	ComponentEventSystem & operator=(const ComponentEventSystem& component_to_copy);
	ComponentEventSystem & operator=(ComponentEventSystem&& component_to_move) = default;

	Component* Clone(GameObject* owner, bool original_prefab) override;
	void CopyTo(Component* component_to_copy) const override;
	void Delete() override;

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

	void Update() override;

private:
	void MousePicking(float2 mouse_position);
};

#endif //_COMPONENTEVENTSYSTEM_H_

