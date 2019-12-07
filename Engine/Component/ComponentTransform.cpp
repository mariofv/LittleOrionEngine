#include "ComponentTransform.h"
#include "GameObject.h"
#include "UI/ComponentsUI.h"

ComponentTransform::ComponentTransform(GameObject * owner) : Component(owner, ComponentType::TRANSFORM) {

	GenerateModelMatrix();
}

ComponentTransform::ComponentTransform(GameObject * owner, const float3 translation, const float3 rotation, const float3 scale) :
	Component(owner, ComponentType::TRANSFORM),
	translation(translation),
	rotation(rotation),
	scale(scale)
{
	GenerateModelMatrix();
}

void ComponentTransform::Enable()
{

}

void ComponentTransform::Disable()
{

}

void ComponentTransform::Update()
{

}

float3 ComponentTransform::GetTranslation() const
{
	return translation;
}

void ComponentTransform::SetTranslation(const float3 translation)
{
	this->translation = translation;
	GenerateModelMatrix(); // TODO: Change this to Update()
}

void ComponentTransform::Translate(const float3 &translation)
{
	this->translation += translation;
	GenerateModelMatrix(); // TODO: Change this to Update()
}

float3 ComponentTransform::GetRotation() const
{
	return rotation;
}

void ComponentTransform::SetRotation(const float3 rotation)
{
	this->rotation = rotation;
	GenerateModelMatrix(); // TODO: Change this to Update()
}

void ComponentTransform::Rotate(const float3 &rotation)
{
	this->rotation += rotation;
	GenerateModelMatrix(); // TODO: Change this to Update()
}

void ComponentTransform::GenerateModelMatrix()
{
	Quat rotation_quat = Quat::FromEulerXYZ(
		math::DegToRad(rotation.x),
		math::DegToRad(rotation.y),
		math::DegToRad(rotation.z)
	);
	model_matrix = float4x4::FromTRS(translation, rotation_quat, scale);
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

void ComponentTransform::ChangeLocalSpace(const float4x4 new_local_space)
{
	model_matrix = new_local_space.Inverted() * global_model_matrix;
	Quat tmp_rotation;
	model_matrix.Decompose(translation, tmp_rotation, scale);
	rotation = tmp_rotation.ToEulerXYZ();
	rotation.x = math::RadToDeg(rotation.x);
	rotation.y = math::RadToDeg(rotation.y);
	rotation.z = math::RadToDeg(rotation.z);
}

void ComponentTransform::ShowComponentWindow()
{
	ComponentsUI::ShowComponentTransformWindow(this);
}