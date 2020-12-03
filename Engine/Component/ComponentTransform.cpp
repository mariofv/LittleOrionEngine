#include "ComponentTransform.h"

#include "Event/Event.h"
#include "Event/EventManager.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Helper/Utils.h"
#include <Brofiler/Brofiler.h>

ComponentTransform::ComponentTransform(ComponentType transform_type) : Component(transform_type)
{
}

ComponentTransform::ComponentTransform(GameObject* owner, ComponentType transform_type) : Component(owner, transform_type)
{
	OnTransformChange();
}

ComponentTransform::ComponentTransform(GameObject* owner, const float3 translation, const Quat rotation, const float3 scale) :
	Component(owner, ComponentType::TRANSFORM),
	translation(translation),
	rotation(rotation),
	scale(scale)
{
	OnTransformChange();
}

void ComponentTransform::CopyTo(Component* component_to_copy) const
{ 
	*component_to_copy = *this;
	*static_cast<ComponentTransform*>(component_to_copy) = *this; 
	static_cast<ComponentTransform*>(component_to_copy)->OnTransformChange();
};

ComponentTransform & ComponentTransform::operator=(const ComponentTransform & component_to_copy)
{
	this->translation = component_to_copy.translation;
	this->rotation = component_to_copy.rotation;
	this->rotation_degrees = component_to_copy.rotation_degrees;
	this->rotation_radians = component_to_copy.rotation_radians;
	this->scale = component_to_copy.scale;

	this->model_matrix = component_to_copy.model_matrix;
	this->global_model_matrix = component_to_copy.global_model_matrix;
	OnTransformChange();
	return *this;
}

void ComponentTransform::SpecializedSave(Config& config) const
{
	config.AddFloat3(translation, "Translation");
	config.AddFloat3(rotation_degrees, "Rotation");
	config.AddFloat3(scale, "Scale");
}

void ComponentTransform::SpecializedLoad(const Config& config)
{
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

void ComponentTransform::SetTranslation(const float3& translation)
{
	BROFILER_CATEGORY("SetTranslation", Profiler::Color::Lavender);
	this->translation = translation;
	OnTransformChange();
}

void ComponentTransform::Translate(const float3& translation)
{
	this->translation += translation;
	OnTransformChange();
}

void ComponentTransform::SetGlobalMatrixTranslation(const float3& translation)
{
	global_model_matrix.SetTranslatePart(translation);
 	SetGlobalModelMatrix(global_model_matrix);
}

Quat ComponentTransform::GetGlobalRotation() const
{
	return global_model_matrix.RotatePart().ToQuat();
}

Quat ComponentTransform::GetRotation() const
{
	return rotation;
}

float3 ComponentTransform::GetRotationRadiants() const
{
	return rotation_radians;
}

void ComponentTransform::SetRotation(const float3x3& rotation)
{
	this->rotation = rotation.ToQuat();
	rotation_radians = rotation.ToEulerXYZ();
	rotation_degrees = Utils::Float3RadToDeg(rotation_radians);
	OnTransformChange();
}

/*
	@param new_rotation The rotation in degrees
*/
void ComponentTransform::SetRotation(const float3& new_rotation)
{
	rotation_radians = Utils::Float3DegToRad(new_rotation);
	rotation = math::Quat::FromEulerXYZ(rotation_radians.x, rotation_radians.y, rotation_radians.z);
	rotation_degrees = new_rotation;
	OnTransformChange();
}

void ComponentTransform::SetRotationRad(const float3& rotation_radians)
{
	this->rotation_radians = rotation_radians;
	rotation = math::Quat::FromEulerXYZ(rotation_radians.x, rotation_radians.y, rotation_radians.z);
	rotation_degrees = Utils::Float3RadToDeg(rotation_radians);
	OnTransformChange();
}

void ComponentTransform::SetRotation(const Quat& new_rotation)
{
	rotation = new_rotation;
	rotation_radians = new_rotation.ToEulerXYZ();
	rotation_degrees = Utils::Float3RadToDeg(rotation_radians);
	OnTransformChange();
}

void ComponentTransform::SetGlobalMatrixRotation(const float3x3& rotation)
{
	global_model_matrix.SetRotatePart(rotation);
	SetGlobalModelMatrix(global_model_matrix);
}

void ComponentTransform::SetGlobalMatrixRotation(const Quat& rotation)
{
	global_model_matrix.SetRotatePart(rotation);
	SetGlobalModelMatrix(global_model_matrix);
}

void ComponentTransform::Rotate(const Quat& rotation)
{
	this->rotation = rotation * this->rotation;
	rotation_radians = rotation * rotation_radians;
	rotation_degrees = Utils::Float3RadToDeg(rotation_radians);

	OnTransformChange();
}

void ComponentTransform::Rotate(const float3x3& rotation)
{
	this->rotation = rotation.ToQuat() * this->rotation;
	rotation_radians = rotation * rotation_radians;
	rotation_degrees = Utils::Float3RadToDeg(rotation_radians);

	OnTransformChange();
}

void ComponentTransform::LookAt(const float3& target)
{
	float3 direction = (target - GetGlobalTranslation());
	Quat new_rotation = GetGlobalRotation().LookAt(float3::unitZ, direction.Normalized(), float3::unitY, float3::unitY);
	SetGlobalMatrixRotation(new_rotation);
}

float3 ComponentTransform::ComponentTransform::GetScale() const
{
	return scale;
}

void ComponentTransform::SetScale(const float3& scale)
{
	this->scale = scale;
	
	OnTransformChange();
}

 void ComponentTransform::SetGlobalMatrixScale(const float3& scale)
{
	 global_model_matrix = float4x4::FromTRS(GetGlobalTranslation(), GetGlobalRotation(), scale);
	 SetGlobalModelMatrix(global_model_matrix);
}

float3 ComponentTransform::GetGlobalScale() const
{
	return global_model_matrix.GetScale();
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

ENGINE_API float3 ComponentTransform::GetGlobalUpVector() const
{
	return global_model_matrix.RotatePart() * float3::unitY;
}

ENGINE_API float3 ComponentTransform::GetGlobalFrontVector() const
{
	return global_model_matrix.RotatePart() * float3::unitZ;
}

ENGINE_API float3 ComponentTransform::GetGlobalRightVector() const
{
	return Cross(GetGlobalFrontVector(), GetGlobalUpVector());
}

void ComponentTransform::OnTransformChange()
{
	BROFILER_CATEGORY("OnTransformChange", Profiler::Color::Lavender);
	model_matrix = float4x4::FromTRS(translation, rotation, scale);
	GenerateGlobalModelMatrix();
	for (const auto& child : owner->children)
	{
		child->transform.OnTransformChange();
	}
	owner->aabb.GenerateBoundingBox();
}

float4x4 ComponentTransform::GetModelMatrix() const
{
	return model_matrix;
}

void ComponentTransform::GenerateGlobalModelMatrix()
{
	BROFILER_CATEGORY("GenerateGlobalMatrix", Profiler::Color::Lavender);
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

void ComponentTransform::SetGlobalModelMatrix(const float4x4& new_global_matrix)
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

void ComponentTransform::ChangeLocalSpace(const float4x4& new_local_space)
{
	model_matrix = new_local_space.Inverted() * global_model_matrix;
	model_matrix.Decompose(translation, rotation, scale);
	OnTransformChange();
}

Component* ComponentTransform::Clone(GameObject* owner, bool /*original_prefab*/)
{
	CopyTo(&owner->transform);
	return &owner->transform;
}
