#include "ComponentTransform2D.h"

#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Helper/Utils.h"
#include <Brofiler/Brofiler.h>

ComponentTransform2D::ComponentTransform2D(GameObject * owner) : Component(owner, ComponentType::TRANSFORM2D) {

	OnTransformChange();
}

ComponentTransform2D::ComponentTransform2D(GameObject * owner, const float2 translation, const float rotation) :
	Component(owner, ComponentType::TRANSFORM2D),
	rotation_degrees(rotation)
{
	rect->x = translation.x;
	rect->y = translation.y;
	rect->w = 0.2f;
	rect->h = 0.2f;
	OnTransformChange();
}

void ComponentTransform2D::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddBool(active, "Active");
	config.AddRect(*rect, "Rect");
	config.AddFloat(rotation_degrees, "Rotation");
}

void ComponentTransform2D::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	config.GetRect("Rect", *rect, SDL_Rect());
	config.GetFloat("Rotation", rotation_degrees);
	rotation_radians = math::DegToRad(rotation_degrees);

	OnTransformChange();
}

float2 ComponentTransform2D::GetGlobalTranslation() const
{
	return global_translation;
}

float2 ComponentTransform2D::GetTranslation() const
{
	return translation;
}

void ComponentTransform2D::SetTranslation(const float2 &translation)
{
	this->translation = translation;
	OnTransformChange();
}

void ComponentTransform2D::Translate(const float2 &translation)
{
	this->translation += translation;
	OnTransformChange();
}

float ComponentTransform2D::GetRotationRadiants() const
{
	return rotation_radians;
}

float ComponentTransform2D::GetRotation() const
{
	return rotation_degrees;
}

void ComponentTransform2D::SetRotation(const float &rotation)
{
	rotation_degrees = rotation;
	rotation_radians = math::DegToRad(rotation_degrees);

	OnTransformChange();
}

void ComponentTransform2D::Rotate(const float &rotation)
{
	rotation_degrees += rotation;
	rotation_radians = math::DegToRad(rotation_degrees);

	OnTransformChange();
}

float2 ComponentTransform2D::ComponentTransform2D::GetScale() const
{
	return scale;
}


void ComponentTransform2D::SetScale(const float2 &scale)
{
	this->scale = scale;
	OnTransformChange();
}

void ComponentTransform2D::OnTransformChange()
{
	//If I have a parent
	ComponentTransform2D* parent = nullptr;
	if (owner != nullptr && (parent = ((ComponentTransform2D*)owner->GetComponent(ComponentType::TRANSFORM2D))) != nullptr)
	{
		translation = parent->translation + translation;
	}
	else // I am the parent
	{

	}
}

void ComponentTransform2D::OnTransformChange(ComponentTransform2D &parent)
{
	for (auto & child : owner->children)
	{
		((ComponentTransform2D*)child->GetComponent(ComponentType::TRANSFORM2D))->OnTransformChange(*this);
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
		global_model_matrix = owner->parent->transform.global_model_matrix * model_matrix;
	}
}

float4x4 ComponentTransform2D::GetGlobalModelMatrix() const
{
	return global_model_matrix;
}

void ComponentTransform2D::SetGlobalModelMatrix(const float4x4 &new_global_matrix)
{
	if (owner->parent == nullptr)
	{
		model_matrix = new_global_matrix;
	}
	else
	{
		model_matrix = owner->parent->transform.global_model_matrix.Inverted() * new_global_matrix;
	}

	float3 translation, scale;
	float3x3 rotation;

	model_matrix.Decompose(translation, rotation, scale);

	SetTranslation(translation);
	SetRotation(rotation);
	SetScale(scale);

}

void ComponentTransform2D::ChangeLocalSpace(const float4x4 &new_local_space)
{
	model_matrix = new_local_space.Inverted() * global_model_matrix;
	model_matrix.Decompose(translation, rotation, scale);
}