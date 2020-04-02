#ifndef _COMPONENTTRANSFORM2D_H_
#define _COMPONENTTRANSFORM2D_H_

#define ENGINE_EXPORTS

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

	// Heredado vía Component
	virtual void Delete() override;
	virtual Component * Clone(bool create_on_module = true) const override;
	virtual void Copy(Component * component_to_copy) const override;
	virtual void Save(Config & config) const override;
	virtual void Load(const Config & config) override;

	void GenerateGlobalModelMatrix();
	void SetSize(float new_width, float new_height);
	ENGINE_API void SetPosition(float x, float y);
	ENGINE_API void SetPosition(float3* new_position);
	void CalculateRectMatix(float new_width, float new_height, float4x4* matrix);
	void CalculateTxtRectMatix(float x, float y, float width, float height, float4x4* matrix);
private:
	void OnTransformChange();
	void UpdateRect();
	friend class PanelComponent;

public:
	float width = 10;
	float height = 10;
	bool is_new = true;
	math::Rect rect = math::Rect(0, 0, 10, 10);
	float rotation = 0.0f;
	float3 position = float3::zero;
	float2 scale = float2::one;
	float4x4 global_matrix = float4x4::identity;
	float4x4 model_matrix = float4x4::identity;
	float4x4 rect_matrix = float4x4::identity;
};
#endif

