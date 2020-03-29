#ifndef _COMPONENTTRANSFORM2D_H_
#define _COMPONENTTRANSFORM2D_H_
#include "Component.h"
class ComponentTransform2D : public Component
{
public:
	ComponentTransform2D();
	ComponentTransform2D(GameObject * owner);
	ComponentTransform2D(GameObject * owner, const Rect rect, const float rotation);
	~ComponentTransform2D();

	//Copy and move
	ComponentTransform2D(const ComponentTransform2D& component_to_copy) = default;
	ComponentTransform2D(ComponentTransform2D&& component_to_move) = default;

	ComponentTransform2D & operator=(const ComponentTransform2D & component_to_copy);
	ComponentTransform2D & operator=(ComponentTransform2D && component_to_move) = default;

	math::Rect rect = math::Rect(0, 0, 10, 10);
	float rotation = 0.0f;
	float2 position = float2::zero;
	float2 scale = float2::one;
	float4x4 scale_matrix = float4x4::identity;

	// Heredado vía Component
	virtual void Delete() override;
	virtual Component * Clone(bool create_on_module = true) const override;
	virtual void Copy(Component * component_to_copy) const override;
	virtual void Save(Config & config) const override;
	virtual void Load(const Config & config) override;

	void SetSize(float width, float height);
private:
	void OnTransformChange();

	friend class PanelComponent;
};
#endif

