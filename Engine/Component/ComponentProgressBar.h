#ifndef _COMPONENTPROGRESSBAR_H_
#define _COMPONENTPROGRESSBAR_H_

#include "Component.h"

class ComponentProgressBar : public Component
{
public:
	ComponentProgressBar();
	ComponentProgressBar(GameObject * owner);
	~ComponentProgressBar() = default;

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	void Delete() override;
	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

public:
	float percentage = 20.0F;
	unsigned int bar_texture = 2;
	float3 bar_color = float3::unitX;
};
#endif