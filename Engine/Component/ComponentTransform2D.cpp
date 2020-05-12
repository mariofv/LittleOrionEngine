#include "ComponentTransform2D.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleWindow.h"

const std::array<AnchorPreset, 16> ComponentTransform2D::anchor_presets =
{
	AnchorPreset{ AnchorPreset::AnchorPresetType::LEFT_HORIZONTAL_TOP_VERTICAL, float2(0.f, 1.f), float2(0.f, 1.f) },
	AnchorPreset{ AnchorPreset::AnchorPresetType::CENTER_HORIZONTAL_TOP_VERTICAL, float2(0.5f, 1.f), float2(0.5f, 1.f) },
	AnchorPreset{ AnchorPreset::AnchorPresetType::RIGHT_HORIZONTAL_TOP_VERTICAL, float2(1.f, 1.f), float2(1.f, 1.f) },
	AnchorPreset{ AnchorPreset::AnchorPresetType::STRETCH_HORIZONTAL_TOP_VERTICAL, float2(0.f, 1.f), float2(1.f, 1.f) },

	AnchorPreset{ AnchorPreset::AnchorPresetType::LEFT_HORIZONTAL_CENTER_VERTICAL, float2(0.f, 0.5f), float2(0.f, 0.5f) },
	AnchorPreset{ AnchorPreset::AnchorPresetType::CENTER_HORIZONTAL_CENTER_VERTICAL, float2(0.5f, 0.5f), float2(0.5f, 0.5f) },
	AnchorPreset{ AnchorPreset::AnchorPresetType::RIGHT_HORIZONTAL_CENTER_VERTICAL, float2(1.f, 0.5f), float2(1.f, 0.5f) },
	AnchorPreset{ AnchorPreset::AnchorPresetType::STRETCH_HORIZONTAL_CENTER_VERTICAL, float2(0.f, 0.5f), float2(1.f, 0.5f) },

	AnchorPreset{ AnchorPreset::AnchorPresetType::LEFT_HORIZONTAL_BOTTOM_VERTICAL, float2(0.f, 0.f), float2(0.f, 0.f) },
	AnchorPreset{ AnchorPreset::AnchorPresetType::CENTER_HORIZONTAL_BOTTOM_VERTICAL, float2(0.5f, 0.f), float2(0.5f, 0.f) },
	AnchorPreset{ AnchorPreset::AnchorPresetType::RIGHT_HORIZONTAL_BOTTOM_VERTICAL, float2(1.f, 0.f), float2(1.f, 0.f) },
	AnchorPreset{ AnchorPreset::AnchorPresetType::STRETCH_HORIZONTAL_BOTTOM_VERTICAL, float2(0.f, 0.f), float2(1.f, 0.f) },

	AnchorPreset{ AnchorPreset::AnchorPresetType::LEFT_HORIZONTAL_STRETCH_VERTICAL, float2(0.f, 0.f), float2(0.f, 1.f) },
	AnchorPreset{ AnchorPreset::AnchorPresetType::CENTER_HORIZONTAL_STRETCH_VERTICAL, float2(0.5f, 0.f), float2(0.5f, 1.f) },
	AnchorPreset{ AnchorPreset::AnchorPresetType::RIGHT_HORIZONTAL_STRETCH_VERTICAL, float2(1.f, 0.f), float2(1.f, 1.f) },
	AnchorPreset{ AnchorPreset::AnchorPresetType::STRETCH_HORIZONTAL_STRETCH_VERTICAL, float2(0.f, 0.f), float2(1.f, 1.f) }
};

ComponentTransform2D::ComponentTransform2D() : ComponentTransform(ComponentType::TRANSFORM2D)
{
}

ComponentTransform2D::ComponentTransform2D(GameObject * owner) : ComponentTransform(owner, ComponentType::TRANSFORM2D)
{
	OnTransformChange();
}

ComponentTransform2D::~ComponentTransform2D()
{
}

void ComponentTransform2D::Delete()
{
}

Component * ComponentTransform2D::Clone(bool create_on_module) const
{
	ComponentTransform2D * created_component;
	created_component = new ComponentTransform2D(nullptr);
	*created_component = *this;
	return created_component;
}

void ComponentTransform2D::Copy(Component * component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentTransform2D*>(component_to_copy) = *this;
};

ComponentTransform2D & ComponentTransform2D::operator=(const ComponentTransform2D & component_to_copy)
{
	this->translation = component_to_copy.translation;
	this->rotation = component_to_copy.rotation;
	this->rotation_degrees = component_to_copy.rotation_degrees;
	this->rotation_radians = component_to_copy.rotation_radians;
	this->scale = component_to_copy.scale;

	this->model_matrix = component_to_copy.model_matrix;

	size = component_to_copy.size;
	anchored_position = component_to_copy.anchored_position;
	min_anchor = component_to_copy.min_anchor;
	max_anchor = component_to_copy.max_anchor;

	OnTransformChange();
	return *this;
}

void ComponentTransform2D::SpecializedSave(Config& config) const
{
	config.AddFloat2(size, "Size");
	config.AddFloat2(pivot, "Pivot");
	config.AddFloat2(anchored_position, "AnchoredPos");

	config.AddFloat2(min_anchor, "MinAnchor");
	config.AddFloat2(max_anchor, "MaxAnchor");
}

void ComponentTransform2D::SpecializedLoad(const Config& config)
{	
	config.GetFloat2("Size", size, float2(100.f));
	config.GetFloat2("Pivot", pivot, float2(0.5f));
	config.GetFloat2("AnchoredPos", anchored_position, float2::zero);

	config.GetFloat2("MinAnchor", min_anchor, float2(0.5f));
	config.GetFloat2("MaxAnchor", max_anchor, float2(0.5f));

	OnTransformChange();
}

void ComponentTransform2D::OnTransformChange()
{
	GenerateAnchorPosition();
	GeneratePivotPosition();
	translation = float3(anchored_position + anchor_position, translation.z);
	GenerateRect();
	model_matrix = 
		float4x4::Translate(translation) 
		* float4x4(rotation) 
		* float4x4::Scale(scale)
		* float4x4::Translate(float3(-pivot_position, 0.f))
	;
	GenerateGlobalModelMatrix();

	for (const auto& child : owner->children)
	{
		child->transform_2d.OnTransformChange();
	}
}

void ComponentTransform2D::GenerateGlobalModelMatrix()
{
	if (owner->parent == nullptr)
	{
		global_model_matrix = model_matrix;
	}
	else
	{
		global_model_matrix = owner->parent->transform_2d.global_model_matrix * model_matrix;
	}
}

float4x4 ComponentTransform2D::GetSizedGlobalModelMatrix() const
{
	//return float4x4::Scale(float3(size, 1.f), GetGlobalTranslation()) * global_model_matrix;
	return global_model_matrix * float4x4::Scale(float3(size, 1.f));
}

ENGINE_API void ComponentTransform2D::SetTranslation(const float3& translation)
{
	this->translation = translation;
	anchored_position = float2(translation.x, translation.y) - anchor_position;
	OnTransformChange();
}

ENGINE_API void ComponentTransform2D::Translate(const float3& translation)
{
	SetTranslation(this->translation + translation);
}

void ComponentTransform2D::SetWidth(float new_width)
{
	size.x = new_width;
	OnTransformChange();
}

void ComponentTransform2D::SetHeight(float new_height)
{
	size.y = new_height;
	OnTransformChange();
}

void ComponentTransform2D::SetSize(float2 new_size)
{
	size = new_size;
	OnTransformChange();
}

void ComponentTransform2D::SetPivot(const float2& new_pivot)
{
	pivot = new_pivot;
	float2 new_pivot_position = ComputePivotPosition(new_pivot);
	ChangePivotSpace(new_pivot_position);
}

void ComponentTransform2D::ChangePivotSpace(const float2& new_pivot_position)
{
	anchored_position = anchored_position - pivot_position + new_pivot_position;
	OnTransformChange();
}

void ComponentTransform2D::GeneratePivotPosition()
{
	pivot_position = ComputePivotPosition(pivot);
}

float2 ComponentTransform2D::ComputePivotPosition(float2 pivot_point)
{
	float pivot_position_x = size.x * pivot_point.x - size.x * 0.5f;
	float pivot_position_y = size.y * pivot_point.y - size.y * 0.5f;

	return float2(pivot_position_x, pivot_position_y);
}


void ComponentTransform2D::SetAnchorPreset(AnchorPreset::AnchorPresetType new_anchor_preset)
{
	for (auto& anchor_preset_struct : anchor_presets)
	{
		if (anchor_preset_struct.type == new_anchor_preset)
		{
			anchor_preset = new_anchor_preset;
			SetMinAnchor(anchor_preset_struct.min_anchor);
			SetMaxAnchor(anchor_preset_struct.max_anchor);
			return;
		}
	}
}

AnchorPreset::AnchorPresetType ComponentTransform2D::GetAnchorPreset() const
{
	return anchor_preset;
}

void ComponentTransform2D::UpdateAnchorPreset()
{
	for (auto& anchor_preset_struct : anchor_presets)
	{
		if (
			anchor_preset_struct.min_anchor.x == min_anchor.x
			&& anchor_preset_struct.min_anchor.y == min_anchor.y
			&& anchor_preset_struct.max_anchor.x == max_anchor.x
			&& anchor_preset_struct.max_anchor.y == max_anchor.y
		)
		{
			anchor_preset = anchor_preset_struct.type;
			return;
		}
	}

	anchor_preset = AnchorPreset::AnchorPresetType::CUSTOM;
}

void ComponentTransform2D::SetMinAnchor(const float2& new_min_anchor)
{
	min_anchor = new_min_anchor;
	float2 new_anchor_position = ComputeAnchorPosition(new_min_anchor, max_anchor);
	ChangeAnchorSpace(new_anchor_position);
	UpdateAnchorPreset();
}

void ComponentTransform2D::SetMaxAnchor(const float2& new_max_anchor)
{
	max_anchor = new_max_anchor;
	float2 new_anchor_position = ComputeAnchorPosition(min_anchor, new_max_anchor);
	ChangeAnchorSpace(new_anchor_position);
	UpdateAnchorPreset();
}

void ComponentTransform2D::GenerateAnchorPosition()
{
	anchor_position = ComputeAnchorPosition(min_anchor, max_anchor);
}

float2 ComponentTransform2D::ComputeAnchorPosition(float2 minimum_anchor, float2 maximum_anchor)
{
	if (owner->parent != nullptr)
	{
		float2 parent_size = owner->parent->transform_2d.size;
		float anchored_position_x = parent_size.x * (maximum_anchor.x + minimum_anchor.x - 1) / 2.f;
		float anchored_position_y = parent_size.y * (maximum_anchor.y + minimum_anchor.y - 1) / 2.f;

		return float2(anchored_position_x, anchored_position_y);
	}
	else
	{
		return float2::zero;
	}
}

void ComponentTransform2D::ChangeAnchorSpace(const float2& new_anchor_position)
{
	anchored_position = anchored_position + anchor_position - new_anchor_position;
	OnTransformChange();
}

bool ComponentTransform2D::HasCoincidentHorizontalAnchors() const
{
	return min_anchor.x == max_anchor.x;
}

bool ComponentTransform2D::HasCoincidentVerticalAnchors() const
{
	return min_anchor.y == max_anchor.y;
}

void ComponentTransform2D::GenerateRect()
{
	float2 parent_size = owner->parent != nullptr ? owner->parent->transform_2d.size : float2::zero;

	rect.left = translation.x - size.x / 2.f - parent_size.x * min_anchor.x + parent_size.x * 0.5f;
	rect.right = parent_size.x * max_anchor.x - (translation.x + size.x / 2.f) - parent_size.x * 0.5f;
	assert(math::RoundInt(rect.left + size.x + rect.right) == math::RoundInt(parent_size.x * (max_anchor.x - min_anchor.x)));

	rect.bottom = translation.y - size.y / 2.f - parent_size.y * min_anchor.y + parent_size.y * 0.5f;
	rect.top = parent_size.y * max_anchor.y - (translation.y + size.y / 2.f) - parent_size.y * 0.5f;
	assert(math::RoundInt(rect.bottom + size.y + rect.top) == math::RoundInt(parent_size.y * (max_anchor.y - min_anchor.y)));
}

void ComponentTransform2D::SetLeft(float left)
{
	float delta_left = left - rect.left;
	rect.left = left;
	anchored_position.x += delta_left * 0.5f;
	size.x -= delta_left;
	OnTransformChange();
}

void ComponentTransform2D::SetRight(float right)
{
	float delta_right = right - rect.right;
	rect.right = right;
	anchored_position.x -= delta_right * 0.5f;
	size.x -= delta_right;
	OnTransformChange();
}

void ComponentTransform2D::SetBottom(float bottom)
{
	float delta_bottom = bottom - rect.bottom;
	rect.bottom = bottom;
	anchored_position.y += delta_bottom * 0.5f;
	size.y -= delta_bottom;
	OnTransformChange();
}

void ComponentTransform2D::SetTop(float top)
{
	float delta_top = top - rect.top;
	rect.top = top;
	anchored_position.y -= delta_top * 0.5f;
	size.y -= delta_top;
	OnTransformChange();
}