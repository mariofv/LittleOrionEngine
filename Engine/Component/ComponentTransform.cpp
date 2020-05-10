#include "ComponentTransform.h"

#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Helper/Utils.h"
#include <Brofiler/Brofiler.h>

ComponentTransform::ComponentTransform() : Component(ComponentType::TRANSFORM)
{
}

ComponentTransform::ComponentTransform(GameObject * owner) : Component(owner, ComponentType::TRANSFORM)
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
	BROFILER_CATEGORY("SetTranslation", Profiler::Color::Lavender);
	this->translation = translation;
	OnTransformChange();
}

ENGINE_API void ComponentTransform::Translate(const float3& translation)
{
	this->translation += translation;
	OnTransformChange();
}

ENGINE_API void ComponentTransform::SetGlobalMatrixTranslation(const float3& translation)
{
	global_model_matrix.SetTranslatePart(translation);
	SetGlobalModelMatrix(global_model_matrix);
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

ENGINE_API void ComponentTransform::SetRotation(const float3& new_rotation)
{
	rotation = math::Quat::FromEulerXYZ(new_rotation.x, new_rotation.y, new_rotation.z);
	rotation_radians = new_rotation;
	rotation_degrees = Utils::Float3RadToDeg(rotation_radians);
	OnTransformChange();
}

ENGINE_API void ComponentTransform::SetRotation(const Quat& new_rotation)
{
	rotation = new_rotation;
	rotation_radians = new_rotation.ToEulerXYZ();
	rotation_degrees = Utils::Float3RadToDeg(rotation_radians);
	OnTransformChange();
}

ENGINE_API void ComponentTransform::SetGlobalMatrixRotation(const float3x3& rotation)
{
	global_model_matrix.SetRotatePart(rotation);
	SetGlobalModelMatrix(global_model_matrix);
}

ENGINE_API void ComponentTransform::SetGlobalMatrixRotation(const Quat& rotation)
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

ENGINE_API void ComponentTransform::LookAt(const float3& target)
{
	float3 direction = (target - GetTranslation());
	Quat new_rotation = GetRotation().LookAt(float3::unitZ, direction.Normalized(), float3::unitY, float3::unitY);
	SetRotation(new_rotation);
}

ENGINE_API float3 ComponentTransform::ComponentTransform::GetScale() const
{
	return scale;
}

ENGINE_API void ComponentTransform::SetScale(const float3& scale)
{
	this->scale = scale;
	
	OnTransformChange();
}

float3 ComponentTransform::GetGlobalScale() const
{
	return global_model_matrix.GetScale();
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
	
}

Component* ComponentTransform::Clone(bool original_prefab) const
{
	ComponentTransform * created_component;
	created_component = new ComponentTransform(nullptr);
	*created_component = *this;
	return created_component;
}
