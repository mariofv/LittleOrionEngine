#include "ComponentTransform.h"
#include "GameObject.h"
#include "UI/ComponentsUI.h"

ComponentTransform::ComponentTransform(GameObject * owner) : Component(owner, ComponentType::TRANSFORM) {

	GenerateModelMatrix();
}

ComponentTransform::ComponentTransform(GameObject * owner, const float3 translation, const Quat rotation, const float3 scale) :
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

void ComponentTransform::Save(Config& config) const
{

}

void ComponentTransform::Load(const Config& config)
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

Quat ComponentTransform::GetRotation() const
{
	return rotation;
}

void ComponentTransform::SetRotation(const Quat &rotation)
{
	this->rotation = rotation;
	GenerateModelMatrix(); // TODO: Change this to Update()
}

void ComponentTransform::SetRotation(const float3x3 &rotation)
{
	this->rotation = rotation.ToQuat();
	GenerateModelMatrix(); // TODO: Change this to Update()
}

void ComponentTransform::Rotate(const Quat &rotation)
{
	this->rotation = rotation * this->rotation;
	GenerateModelMatrix(); // TODO: Change this to Update()
}

void ComponentTransform::Rotate(const float3x3 &rotation)
{
	this->rotation = rotation.ToQuat() * this->rotation;
	GenerateModelMatrix(); // TODO: Change this to Update()
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

void ComponentTransform::GenerateModelMatrix()
{
	model_matrix = float4x4::FromTRS(translation, rotation, scale);
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
	model_matrix.Decompose(translation, rotation, scale);
	
}

void ComponentTransform::ShowComponentWindow()
{
	ComponentsUI::ShowComponentTransformWindow(this);
}