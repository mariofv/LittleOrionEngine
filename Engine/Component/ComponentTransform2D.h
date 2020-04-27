#ifndef _COMPONENTTRANSFORM2D_H_
#define _COMPONENTTRANSFORM2D_H_

#define ENGINE_EXPORTS

#include "ComponentTransform.h"

class ComponentTransform2D : public ComponentTransform
{
public:
	struct Rect
	{
		float left, right, bottom, top;
	};

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
	void SetSize(float2 new_size);

	void SetPivot(const float2& new_pivot);
	void ChangePivotSpace(const float2& new_pivot_position);

	void GeneratePivotPosition();
	float2 ComputePivotPosition(float2 pivot_point);

	void SetMinAnchor(const float2& new_min_anchor);
	void SetMaxAnchor(const float2& new_min_anchor);
	void ChangeAnchorSpace(const float2& new_anchor_matrix);

	void GenerateAnchorPosition();
	float2 ComputeAnchorPosition(float2 min_anchor, float2 max_anchor);

	void SetLeft(float left);
	void SetRight(float right);
	void SetBottom(float bottom);
	void SetTop(float top);
	void GenerateRect();

private:
	void OnTransformChange() override;

public:
	float2 anchored_position = float2::zero;
	float2 size = float2(100.f, 100.f);
	Rect rect;

private:
	float2 pivot = float2(0.5f);
	float2 pivot_position = float2::zero;

	float2 min_anchor = float2(0.5f);
	float2 max_anchor = float2(0.5f);

	float2 anchor_position = float2::zero;

	friend class PanelComponent;
};
#endif //_COMPONENTTRANSFORM2D_H_