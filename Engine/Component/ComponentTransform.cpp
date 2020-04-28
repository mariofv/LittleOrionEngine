#include "ComponentTransform.h"

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

void ComponentTransform::Copy(Component * component_to_copy) const
{ 
	*component_to_copy = *this;
	*static_cast<ComponentTransform*>(component_to_copy) = *this; 
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

ENGINE_API float3 ComponentTransform::GetGlobalTranslation() const
{
	return global_model_matrix.TranslatePart();
}


ENGINE_API float3 ComponentTransform::GetTranslation() const
{
	return translation;
}

ENGINE_API void ComponentTransform::SetTranslation(const float3& translation)
{
	this->translation = translation;
	OnTransformChange();
}

ENGINE_API void ComponentTransform::Translate(const float3& translation)
{
	this->translation += translation;
	OnTransformChange();
}

ENGINE_API Quat ComponentTransform::GetGlobalRotation() const
{
	return global_model_matrix.RotatePart().ToQuat();
}

ENGINE_API Quat ComponentTransform::GetRotation() const
{
	return rotation;
}

ENGINE_API float3 ComponentTransform::GetRotationRadiants() const
{
	return rotation_radians;
}

ENGINE_API void ComponentTransform::SetRotation(const float3x3& rotation)
{
	this->rotation = rotation.ToQuat();
	rotation_radians = rotation.ToEulerXYZ();
	rotation_degrees = Utils::Float3RadToDeg(rotation_radians);
	OnTransformChange();
}

/*
	@param new_rotation The rotation in degrees
*/
ENGINE_API void ComponentTransform::SetRotation(const float3& new_rotation)
{
	rotation_radians = Utils::Float3DegToRad(new_rotation);
	rotation = math::Quat::FromEulerXYZ(rotation_radians.x, rotation_radians.y, rotation_radians.z);
	rotation_degrees = new_rotation;
	OnTransformChange();
}

ENGINE_API void ComponentTransform::SetRotation(const Quat& new_rotation)
{
	rotation = new_rotation;
	rotation_radians = new_rotation.ToEulerXYZ();
	rotation_degrees = Utils::Float3RadToDeg(rotation_radians);
	OnTransformChange();
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

ENGINE_API void ComponentTransform::LookAt(const float3& target)
{
	float3 direction = (target - GetTranslation());
	Quat new_rotation = GetRotation().LookAt(float3::unitZ, direction.Normalized(), float3::unitY, float3::unitY);
	SetRotation(new_rotation);
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

ENGINE_API float3 ComponentTransform::GetUpVector() const
{
	return rotation * float3::unitY;
}


ENGINE_API float3 ComponentTransform::GetFrontVector() const
{
	return rotation * float3::unitZ;
}

ENGINE_API float3 ComponentTransform::GetRightVector() const
{
	return Cross(GetFrontVector(), GetUpVector());
}

void ComponentTransform::OnTransformChange()
{
	model_matrix = float4x4::FromTRS(translation, rotation, scale);
	GenerateGlobalModelMatrix();
	owner->aabb.GenerateBoundingBox();
	for (const auto& child : owner->children)
	{
		child->transform.OnTransformChange();
	}
}

float4x4 ComponentTransform::GetModelMatrix() const
{
	return model_matrix;
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

Component* ComponentTransform::Clone(bool original_prefab) const
{
	ComponentTransform * created_component;
	created_component = new ComponentTransform(nullptr);
	*created_component = *this;
	return created_component;
}
