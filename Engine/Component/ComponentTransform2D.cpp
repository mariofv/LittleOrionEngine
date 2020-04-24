#include "ComponentTransform2D.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleWindow.h"

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
	config.AddFloat2(anchored_position, "AnchoredPos");

	config.AddFloat2(min_anchor, "MinAnchor");
	config.AddFloat2(max_anchor, "MaxAnchor");
}

void ComponentTransform2D::SpecializedLoad(const Config& config)
{	
	config.GetFloat2("Size", size, float2(100.f));
	config.GetFloat2("AnchoredPos", anchored_position, float2::zero);

	config.GetFloat2("MinAnchor", min_anchor, float2(0.5f));
	config.GetFloat2("MaxAnchor", max_anchor, float2(0.5f));

	OnTransformChange();
}

void ComponentTransform2D::OnTransformChange()
{
	GenerateAnchorPosition();
	translation = float3(anchored_position + anchor_position, translation.z);
	GenerateRect();
	model_matrix = float4x4::FromTRS(translation, rotation, scale);
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
	return float4x4::Scale(float3(size, 1.f), GetGlobalTranslation()) * global_model_matrix;
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

void ComponentTransform2D::SetMinAnchor(float2 new_min_anchor)
{
	min_anchor = new_min_anchor;
	float2 new_anchor_position = ComputeAnchorPosition(new_min_anchor, max_anchor);
	ChangeAnchorSpace(new_anchor_position);
}

void ComponentTransform2D::SetMaxAnchor(float2 new_max_anchor)
{
	max_anchor = new_max_anchor;
	float2 new_anchor_position = ComputeAnchorPosition(min_anchor, new_max_anchor);
	ChangeAnchorSpace(new_anchor_position);
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