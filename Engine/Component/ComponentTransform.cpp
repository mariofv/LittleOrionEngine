#include "ComponentTransform.h"
#include "GameObject.h"
#include "Utils.h"
#include "UI/ComponentsUI.h"
#include "Brofiler/Brofiler.h"

ComponentTransform::ComponentTransform(GameObject * owner) : Component(owner, ComponentType::TRANSFORM) {

	OnTransformUpdate();
}

ComponentTransform::ComponentTransform(GameObject * owner, const float3 translation, const Quat rotation, const float3 scale) :
	Component(owner, ComponentType::TRANSFORM),
	translation(translation),
	rotation(rotation),
	scale(scale)
{
	OnTransformUpdate();
}

void ComponentTransform::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddBool(active, "Active");
	config.AddFloat3(translation, "Translation");
	config.AddQuat(rotation, "Rotation");
	config.AddFloat3(scale, "Scale");
}

void ComponentTransform::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	config.GetFloat3("Translation", translation, float3::zero);
	config.GetQuat("Rotation", rotation, Quat::identity);
	config.GetFloat3("Scale", scale, float3::one);

	OnTransformUpdate();
}

float3 ComponentTransform::GetGlobalTranslation() const
{
	return global_model_matrix.TranslatePart();
}

float3 ComponentTransform::GetTranslation() const
{
	return translation;
}

void ComponentTransform::SetTranslation(const float3 translation)
{
	this->translation = translation;
	OnTransformUpdate();
}

void ComponentTransform::Translate(const float3 &translation)
{
	this->translation += translation;
	OnTransformUpdate();
}

Quat ComponentTransform::GetRotation() const
{
	return rotation;
}

void ComponentTransform::SetRotation(const float3x3 &rotation)
{
	this->rotation = rotation.ToQuat();
	rotation_radians = rotation.ToEulerXYZ();
	rotation_degrees = Utils::Float3RadToDeg(rotation_radians);
	OnTransformUpdate();
}

void ComponentTransform::Rotate(const Quat &rotation)
{
	this->rotation = rotation * this->rotation;
	rotation_radians = rotation * rotation_radians;
	rotation_degrees = Utils::Float3RadToDeg(rotation_radians);

	OnTransformUpdate();
}

void ComponentTransform::Rotate(const float3x3 &rotation)
{
	this->rotation = rotation.ToQuat() * this->rotation;
	rotation_radians = rotation * rotation_radians;
	rotation_degrees = Utils::Float3RadToDeg(rotation_radians);

	OnTransformUpdate();
}

float3 ComponentTransform::ComponentTransform::GetScale() const
{
	return scale;
}


void ComponentTransform::SetScale(const float3 &scale)
{
	this->scale = scale;
	
	OnTransformUpdate();
}

float3 ComponentTransform::GetUpVector() const
{
	return rotation * float3::unitY;
}

float3 ComponentTransform::GetFrontVector() const
{
	return rotation * float3::unitZ;
}

float3 ComponentTransform::GetRightVector() const
{
	return Cross(GetFrontVector(), GetUpVector());
}

void ComponentTransform::OnTransformUpdate()
{
	model_matrix = float4x4::FromTRS(translation, rotation, scale);
	GenerateGlobalModelMatrix();
	owner->aabb.GenerateBoundingBox();
	for (auto & child : owner->children)
	{
		child->transform.OnTransformUpdate();
	}
}

void ComponentTransform::GenerateGlobalModelMatrix()
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

float4x4 ComponentTransform::GetGlobalModelMatrix() const
{
	return global_model_matrix;
}

void ComponentTransform::SetGlobalModelMatrix(const float4x4 &new_global_matrix)
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

void ComponentTransform::ChangeLocalSpace(const float4x4 new_local_space)
{
	model_matrix = new_local_space.Inverted() * global_model_matrix;
	model_matrix.Decompose(translation, rotation, scale);
	
}

void ComponentTransform::ShowComponentWindow()
{
	ComponentsUI::ShowComponentTransformWindow(this);
}