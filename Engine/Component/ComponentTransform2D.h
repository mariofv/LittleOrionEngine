#pragma once
#include "Component.h"
class ComponentTransform2D : public Component
{
public:
	ComponentTransform2D();
	~ComponentTransform2D();

	float2 position;
	float2 size;

	// Heredado vía Component
	virtual void Delete() override;
	virtual Component * Clone(bool create_on_module = true) const override;
	virtual void Copy(Component * component_to_copy) const override;
	virtual void Save(Config & config) const override;
	virtual void Load(const Config & config) override;
};

