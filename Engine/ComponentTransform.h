#ifndef _COMPONENTTRANSFORM_H_
#define _COMPONENTTRANSFORM_H_

#include "Component.h"

#include "MathGeoLib.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform() = default;
	ComponentTransform(const float3 translation, const Quat rotation, const float3 scale);

	~ComponentTransform() = default;

	void Enable() override;
	void Disable() override;
	void Update() override;

private:
	void GenerateModelMatrix();

private:
	float3 translation = float3::zero;
	Quat rotation = Quat::identity;
	float3 scale = float3::one;

	float4x4 model_matrix = float4x4::identity;
};

#endif //_COMPONENTTRANSFORM_H_