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

	size_delta = component_to_copy.size_delta;
	anchored_position = component_to_copy.anchored_position;

	OnTransformChange();
	return *this;
}

void ComponentTransform2D::SpecializedSave(Config& config) const
{
	config.AddFloat(rect.top, "Top");
	config.AddFloat(rect.right, "Right");
	config.AddFloat(rect.bottom, "Bottom");
	config.AddFloat(rect.left, "Left");

	config.AddFloat(size_delta.x, "Width");
	config.AddFloat(size_delta.y, "Height");

	config.AddFloat(min_anchor.x, "MinAnchorX");
	config.AddFloat(min_anchor.y, "MinAnchorY");
	config.AddFloat(max_anchor.x, "MaxAnchorX");
	config.AddFloat(max_anchor.y, "MaxAnchorY");
}

void ComponentTransform2D::SpecializedLoad(const Config& config)
{	
	rect.top = config.GetFloat("Top", 0.0f);
	rect.left = config.GetFloat("Left", 0.0f);
	rect.bottom = config.GetFloat("Bottom", 0.0f);
	rect.right = config.GetFloat("Right", 0.0f);

	size_delta.x = config.GetFloat("Width", 100.0f);
	size_delta.y = config.GetFloat("Height", 100.0f);

	min_anchor.x = config.GetFloat("MinAnchorX", 0.5f);
	min_anchor.y = config.GetFloat("MinAnchorY", 0.5f);
	max_anchor.x = config.GetFloat("MaxAnchorX", 0.5f);
	max_anchor.y = config.GetFloat("MaxAnchorY", 0.5f);

	OnTransformChange();
}

void ComponentTransform2D::OnTransformChange()
{
	GenerateAnchorMatrix();
	anchored_position.z = translation.z;
	translation = (anchor_matrix * float4(anchored_position, 1.f)).Float3Part();
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
	return float4x4::Scale(float3(size_delta, 1.f), GetGlobalTranslation()) * global_model_matrix;
}

void ComponentTransform2D::SetWidth(float new_width)
{
	size_delta.x = new_width;
	OnTransformChange();
}

void ComponentTransform2D::SetHeight(float new_height)
{
	size_delta.y = new_height;
	OnTransformChange();
}

void ComponentTransform2D::SetSizeDelta(float2 new_size)
{
	size_delta = new_size;
	OnTransformChange();
}

void ComponentTransform2D::SetMinAnchor(float2 new_min_anchor)
{
	min_anchor = new_min_anchor;
	float4x4 new_anchor_matrix = ComputeAnchorMatrix(new_min_anchor, max_anchor);
	ChangeAnchorSpace(new_anchor_matrix);
}

void ComponentTransform2D::SetMaxAnchor(float2 new_max_anchor)
{
	max_anchor = new_max_anchor;
	float4x4 new_anchor_matrix = ComputeAnchorMatrix(min_anchor, new_max_anchor);
	ChangeAnchorSpace(new_anchor_matrix);
}

void ComponentTransform2D::GenerateAnchorMatrix()
{
	anchor_matrix = ComputeAnchorMatrix(min_anchor, max_anchor);
}

float4x4 ComponentTransform2D::ComputeAnchorMatrix(float2 minimum_anchor, float2 maximum_anchor)
{
	if (owner->parent != nullptr)
	{
		return float4x4::Translate(
			(minimum_anchor.x - 0.5f) * owner->parent->transform_2d.size_delta.x,
			(minimum_anchor.y - 0.5f) * owner->parent->transform_2d.size_delta.y,
			0.f
		);
	}
	else
	{
		return float4x4::identity;
	}
}

void ComponentTransform2D::ChangeAnchorSpace(const float4x4& new_anchor_matrix)
{
	anchored_position = (new_anchor_matrix.Inverted() * anchor_matrix * float4(anchored_position, 1.f)).Float3Part();
	OnTransformChange();
}

void ComponentTransform2D::SetTranslation(float x, float y)
{
	translation.x = x;
	translation.y = y;
	OnTransformChange();
}