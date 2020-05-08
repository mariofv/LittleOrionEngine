#include "ComponentTransform2D.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleWindow.h"

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
	config.AddFloat3(position, "Position");
	config.AddFloat(width, "Width");
	config.AddFloat(height, "Height");
}

void ComponentTransform2D::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	
	float param = 0.0F;

	rect.top = config.GetFloat("Top", param);
	rect.left = config.GetFloat("Left", param);
	rect.bottom = config.GetFloat("Bottom", param);	
	rect.right = config.GetFloat("Right", param);
	width = config.GetFloat("Width", param);
	height = config.GetFloat("Height", param);
	
	config.GetFloat("Rotation", rotation);

	config.GetFloat2("Scale", scale, float2::one);
	config.GetFloat3("Position", position, float3::zero);

	width = config.GetFloat("Width", 10);
	height = config.GetFloat("Height", 10);

#if GAME
	RescaleTransform();
#endif


	is_new = false;
	OnTransformChange();
}

void ComponentTransform2D::OnTransformChange()
{
	model_matrix = float4x4::FromTRS(position, float4x4::FromEulerXYZ(0, 0, rotation), float3(scale, 1));
	UpdateRect();
	GenerateGlobalModelMatrix();
	CalculateRectMatrix(rect.Width(), rect.Height(), rect_matrix);

	for (const auto& child : owner->children)
	{
		child->transform_2d.OnTransformChange();
	}
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

void ComponentTransform2D::RescaleTransform()
{
	float game_width = App->window->GetWidth();
	float game_height = App->window->GetHeight();

	//This values come from ImGui release/debug versions
	///TODO: think a clever way of get this values
	float scene_width = 1022.0f;
	float scene_height = 597.0f;

	width = width * game_width / scene_width;
	height = height * game_height / scene_height;

	position.x = position.x * game_width / scene_width;
	position.y = position.y * game_height / scene_height;
}

void  ComponentTransform2D::SetSize(float new_width, float new_height)
{
	width = new_width;
	height = new_height;
	OnTransformChange();
}

void ComponentTransform2D::SetPosition(float x, float y)
{
	position.x = x;
	position.y = y;
	OnTransformChange();
}


void ComponentTransform2D::SetPosition(float3* new_position)
{
	position = *new_position;
	OnTransformChange();
}

void ComponentTransform2D::UpdateRect()
{
	rect.left = position.x - width / 2;
	rect.right = position.x + width / 2;
	rect.top = position.y - height / 2;
	rect.bottom = position.y + height / 2;
}

void ComponentTransform2D::CalculateRectMatrix(float new_width, float new_height, float4x4& matrix)
{
	CalculateRectMatrix(rect.left, rect.top, new_width, new_height, matrix);
	
}

void ComponentTransform2D::CalculateRectMatrix(float x, float y, float new_width, float new_height, float4x4& matrix)
{
	matrix = float4x4(global_matrix);
	matrix.SetTranslatePart(float3(x, y, position.z));
	matrix = float4x4::RotateZ(rotation) * matrix;
	matrix = matrix * float4x4::RotateZ(-rotation);
	matrix = matrix * float4x4::Scale(float3(new_width, new_height, 0));

}
