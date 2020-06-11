#ifndef __COMPONENTTRAIL_H_
#define __COMPONENTTRAIL_H_

#include "Component.h"

#include "MathGeoLib.h"
#include "Main/Application.h"
#include "Module/ModuleTime.h"

class GameObject;
class ComponentBillboard;

class ComponentTrail : public Component
{
public:
	ComponentTrail();
	~ComponentTrail();

	ComponentTrail(GameObject* owner);


	void Init();

	ComponentTrail & operator=(const ComponentTrail& component_to_copy);
	ComponentTrail & operator=(ComponentTrail&& component_to_move) = default;

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;
	void Delete() override;

	void Render();

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;
};

#endif