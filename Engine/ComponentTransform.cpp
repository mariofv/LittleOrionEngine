#include "ComponentTransform.h"



ComponentTransform::ComponentTransform(const float3 translation, const Quat rotation, const float3 scale) :
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

void ComponentTransform::GenerateModelMatrix()
{
	model_matrix = float4x4::FromTRS(translation, rotation, scale);
}