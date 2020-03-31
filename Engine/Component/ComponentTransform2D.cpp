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
	config.AddFloat2(scale, "Scale");
	config.AddFloat2(position, "Position");
}

void ComponentTransform2D::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	
	float param = 0.0f;

	rect.top = config.GetFloat("Top", param);
	rect.left = config.GetFloat("Left", param);
	rect.bottom = config.GetFloat("Bottom", param);	
	rect.right = config.GetFloat("Right", param);
	
	config.GetFloat("Rotation", rotation);

	config.GetFloat2("Scale", scale, float2::one);
	config.GetFloat2("Position", position, float2::zero);
	is_new = false;
	OnTransformChange();
}

void ComponentTransform2D::OnTransformChange()
{
	model_matrix = float4x4::FromTRS(float3(position, 0), float4x4::FromEulerXYZ(0, 0, rotation), float3(scale, 0));
	GenerateGlobalModelMatrix();

	for (auto & child : owner->children)
	{
		child->transform_2d.OnTransformChange();
	}

	rect_matrix = float4x4(global_matrix);
	rect_matrix = rect_matrix * float4x4::RotateZ(-rotation); //Reset Rotation
	rect_matrix = rect_matrix * float4x4::Scale(float3(rect.Width(), rect.Height(), 0));
	rect_matrix = float4x4::RotateZ(rotation) * rect_matrix; //Revert Rotation Reset
}

void ComponentTransform2D::GenerateGlobalModelMatrix()
{
	if (owner->parent == nullptr)
	{
		global_matrix = model_matrix;
	}
	else
	{
		global_matrix = owner->parent->transform_2d.global_matrix * model_matrix;
	}
}

void  ComponentTransform2D::SetSize(float width, float height)
{
	rect.right = width;
	rect.bottom = height;
	OnTransformChange();
}

void ComponentTransform2D::SetPosition(float x, float y)
{
	position.x = x;
	position.y = y;
	OnTransformChange();
}
