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

	rect = component_to_copy.rect;
	size = component_to_copy.size;
	
	OnTransformChange();
	return *this;
}

void ComponentTransform2D::SpecializedSave(Config& config) const
{
	config.AddFloat(rect.top, "Top");
	config.AddFloat(rect.right, "Right");
	config.AddFloat(rect.bottom, "Bottom");
	config.AddFloat(rect.left, "Left");

	config.AddFloat(size.x, "Width");
	config.AddFloat(size.y, "Height");
}

void ComponentTransform2D::SpecializedLoad(const Config& config)
{	
	rect.top = config.GetFloat("Top", 0.0f);
	rect.left = config.GetFloat("Left", 0.0f);
	rect.bottom = config.GetFloat("Bottom", 0.0f);
	rect.right = config.GetFloat("Right", 0.0f);

	size.x = config.GetFloat("Width", 100.0f);
	size.y = config.GetFloat("Height", 100.0f);

	OnTransformChange();
}

void ComponentTransform2D::OnTransformChange()
{
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

void ComponentTransform2D::SetTranslation(float x, float y)
{
	translation.x = x;
	translation.y = y;
	OnTransformChange();
}

void ComponentTransform2D::UpdateRect()
{
	rect.left = translation.x - size.x / 2;
	rect.right = translation.x + size.x / 2;
	rect.top = translation.y - size.y / 2;
	rect.bottom = translation.y + size.y / 2;
}