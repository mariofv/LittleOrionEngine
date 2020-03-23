#include "ComponentTransform2D.h"
#include "Main/GameObject.h"

ComponentTransform2D::ComponentTransform2D() : Component(ComponentType::TRANSFORM2D)
{
}

ComponentTransform2D::ComponentTransform2D(GameObject * owner) : Component(owner, ComponentType::TRANSFORM2D)
{
	OnTransformChange();
}

ComponentTransform2D::ComponentTransform2D(GameObject* owner, const float2 translation, const float rotation, const float2 scale) :
	Component(owner, ComponentType::TRANSFORM2D),
	position(translation),
	rotation(rotation),
	size(scale)
{
	OnTransformChange();
}

ComponentTransform2D::~ComponentTransform2D()
{
}

void ComponentTransform2D::Delete()
{
	delete(&position);
	delete(&size);
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
	position = component_to_copy.position;
	rotation = component_to_copy.rotation;
	size = component_to_copy.size;
	
	OnTransformChange();
	return *this;
}

void ComponentTransform2D::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddBool(active, "Active");
	config.AddFloat2(position, "Position");
	config.AddFloat(rotation, "Rotation");
	config.AddFloat2(size, "Scale");
}

void ComponentTransform2D::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	config.GetFloat2("Translation", position, float2::zero);
	config.GetFloat("Rotation", rotation);
	config.GetFloat2("Scale", size, float2::one);

	OnTransformChange();
}

void ComponentTransform2D::OnTransformChange()
{
	for (auto & child : owner->children)
	{
		child->transform_2d.OnTransformChange();
	}
}
