#include "ComponentTransform.h"

#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Helper/Utils.h"
#include <Brofiler/Brofiler.h>

ComponentTransform::ComponentTransform(GameObject * owner) : Component(owner, ComponentType::TRANSFORM) {

	OnTransformChange();
}

ComponentTransform::ComponentTransform(GameObject * owner, const float3 translation, const Quat rotation, const float3 scale) :
	Component(owner, ComponentType::TRANSFORM),
	translation(translation),
	rotation(rotation),
	scale(scale)
{
	OnTransformChange();
}

void ComponentTransform::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddBool(active, "Active");
	config.AddFloat3(translation, "Translation");
	config.AddFloat3(rotation_degrees, "Rotation");
	config.AddFloat3(scale, "Scale");
}

void ComponentTransform::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	config.GetFloat3("Translation", translation, float3::zero);
	config.GetFloat3("Rotation", rotation_degrees, float3::zero);
	rotation = Utils::GenerateQuatFromDegFloat3(rotation_degrees);
	rotation_radians = Utils::Float3DegToRad(rotation_degrees);
	config.GetFloat3("Scale", scale, float3::one);

	OnTransformChange();
}

float3 ComponentTransform::GetGlobalTranslation() const
{
	return global_model_matrix.TranslatePart();
}

float3 ComponentTransform::GetTranslation() const
{
	return translation;
}

void ComponentTransform::SetTranslation(const float3 &translation)
{
	this->translation = translation;
	OnTransformChange();
}

void ComponentTransform::Translate(const float3 &translation)
{
	this->translation += translation;
	OnTransformChange();
}

Quat ComponentTransform::GetRotation() const
{
	return rotation;
}

float3 ComponentTransform::GetRotationRadiants() const
{
	return rotation_radians;
}

void ComponentTransform::SetRotation(const float3x3 &rotation)
{
	this->rotation = rotation.ToQuat();
	rotation_radians = rotation.ToEulerXYZ();
	rotation_degrees = Utils::Float3RadToDeg(rotation_radians);
	OnTransformChange();
}

void ComponentTransform::SetRotation(const float3 & new_rotation)
{
	rotation = math::Quat::FromEulerXYZ(new_rotation.x, new_rotation.y, new_rotation.z);
	rotation_radians = new_rotation;
	rotation_degrees = Utils::Float3RadToDeg(rotation_radians);
	OnTransformChange();
}

void ComponentTransform::Rotate(const Quat &rotation)
{
	this->rotation = rotation * this->rotation;
	rotation_radians = rotation * rotation_radians;
	rotation_degrees = Utils::Float3RadToDeg(rotation_radians);

	OnTransformChange();
}



void ComponentTransform::Rotate(const float3x3 &rotation)
{
	this->rotation = rotation.ToQuat() * this->rotation;
	rotation_radians = rotation * rotation_radians;
	rotation_degrees = Utils::Float3RadToDeg(rotation_radians);

	OnTransformChange();
}

float3 ComponentTransform::ComponentTransform::GetScale() const
{
	return scale;
}


void ComponentTransform::SetScale(const float3 &scale)
{
	this->scale = scale;
	
	OnTransformChange();
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

void ComponentTransform::OnTransformChange()
{
	model_matrix = float4x4::FromTRS(translation, rotation, scale);
	GenerateGlobalModelMatrix();
	owner->aabb.GenerateBoundingBox();
	for (auto & child : owner->children)
	{
		child->transform.OnTransformChange();
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

void ComponentTransform::ChangeLocalSpace(const float4x4 &new_local_space)
{
	model_matrix = new_local_space.Inverted() * global_model_matrix;
	model_matrix.Decompose(translation, rotation, scale);
	
}