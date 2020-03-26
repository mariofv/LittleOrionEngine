#include "ComponentTransform2D.h"
#include "Main/GameObject.h"

ComponentTransform2D::ComponentTransform2D() : Component(ComponentType::TRANSFORM2D)
{
}

ComponentTransform2D::ComponentTransform2D(GameObject * owner) : Component(owner, ComponentType::TRANSFORM2D)
{
	OnTransformChange();
}

ComponentTransform2D::ComponentTransform2D(GameObject* owner, const Rect rext, const float rotation) :
	Component(owner, ComponentType::TRANSFORM2D),
	rotation(rotation),
	rect(rext)
{
		OnTransformChange();
}

ComponentTransform2D::~ComponentTransform2D()
{
}

void ComponentTransform2D::Delete()
{
	delete(&rect);
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
	rect = component_to_copy.rect;
	rotation = component_to_copy.rotation;
	
	OnTransformChange();
	return *this;
}

void ComponentTransform2D::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddBool(active, "Active");
	config.AddFloat(rect.top, "Top");
	config.AddFloat(rect.right, "Right");
	config.AddFloat(rect.bottom, "Bottom");
	config.AddFloat(rect.left, "Left");
	config.AddFloat(rotation, "Rotation");
}

void ComponentTransform2D::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	
	float param = 0.0f;

	config.GetFloat("Top", param);
	rect.top = param;
	
	config.GetFloat("Left", param);
	rect.left = param;
	
	config.GetFloat("Bottom", param);
	rect.bottom = param;
	
	config.GetFloat("Right", param);
	rect.right = param;
	
	config.GetFloat("Rotation", rotation);

	OnTransformChange();
}

void ComponentTransform2D::OnTransformChange()
{
	owner->transform.SetRotation(float3(0, 0, rotation));
	owner->transform.SetTranslation(float3(position, 0));
	owner->transform.SetScale(float3(scale, 0));

	scale_matrix = float4x4::Scale(float3(rect.Width(), rect.Height(), 0), float3::zero);
	scale_matrix = owner->transform.GetGlobalModelMatrix() * scale_matrix;

	for (auto & child : owner->children)
	{
		child->transform_2d.OnTransformChange();
	}
}
