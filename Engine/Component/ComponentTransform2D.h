#ifndef _COMPONENTTRANSFORM2D_H_
#define _COMPONENTTRANSFORM2D_H_

#define ENGINE_EXPORTS

#include "ComponentTransform.h"

class ComponentTransform2D : public ComponentTransform
{
public:
	ComponentTransform2D();
	ComponentTransform2D(GameObject * owner);
	~ComponentTransform2D();

	//Copy and move
	ComponentTransform2D(const ComponentTransform2D& component_to_copy) = default;
	ComponentTransform2D(ComponentTransform2D&& component_to_move) = default;

	ComponentTransform2D & operator=(const ComponentTransform2D & component_to_copy);
	ComponentTransform2D & operator=(ComponentTransform2D && component_to_move) = default;

	Component * Clone(bool create_on_module = true) const override;
	void Copy(Component * component_to_copy) const override;

	void Delete() override;

	void SpecializedSave(Config & config) const override;
	void SpecializedLoad(const Config & config) override;

	void GenerateGlobalModelMatrix();
	float4x4 GetSizedGlobalModelMatrix() const;

	void SetWidth(float new_width);
	void SetHeight(float new_height);
	void SetSizeDelta(float2 new_size);

	void SetMinAnchor(float2 new_min_anchor);
	void SetMaxAnchor(float2 new_min_anchor);
	void ChangeAnchorSpace(const float4x4& new_anchor_matrix);

	void GenerateAnchorMatrix();
	float4x4 ComputeAnchorMatrix(float2 min_anchor, float2 max_anchor);

	ENGINE_API void SetTranslation(float x, float y);

private:
	void OnTransformChange() override;
	void UpdateRect();

public:
	float3 anchored_position = float3::zero;
	float2 size_delta = float2(100.f, 100.f);
	math::Rect rect;

private:
	float2 min_anchor = float2(0.5f);
	float2 max_anchor = float2(0.5f);
	float4x4 anchor_matrix = float4x4::identity;

	friend class PanelComponent;
};
#endif

