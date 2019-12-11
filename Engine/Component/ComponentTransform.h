#ifndef _COMPONENTTRANSFORM_H_
#define _COMPONENTTRANSFORM_H_

#include "Component.h"
#include "UI/ComponentsUI.h"

#include "MathGeoLib.h"
#include <GL/glew.h>

class ComponentTransform : public Component
{
public:
	ComponentTransform(GameObject * owner);
	ComponentTransform(GameObject * owner,const float3 translation, const Quat rotation, const float3 scale);

	~ComponentTransform() = default;

	void Enable() override;
	void Disable() override;
	void Update() override;
	void Delete() override {};
	
	float3 GetTranslation() const;
	void SetTranslation(const float3 translation);
	void Translate(const float3 &translation);

	Quat GetRotation() const;
	void SetRotation(const Quat &rotation);
	void SetRotation(const float3x3 &rotation);
	void Rotate(const Quat &rotation);
	void Rotate(const float3x3 &rotation);

	float3 GetUpVector() const;
	float3 GetFrontVector() const;
	float3 GetRightVector() const;

	void GenerateGlobalModelMatrix();
	void ChangeLocalSpace(const float4x4 new_local_space);

	float4x4 GetGlobalModelMatrix() const;

	void ShowComponentWindow() override;

private:
	void GenerateModelMatrix();

private:
	float3 translation = float3::zero;
	Quat rotation = Quat::identity;
	float3 scale = float3::one;

	float4x4 model_matrix = float4x4::identity;
	float4x4 global_model_matrix = float4x4::identity;

	friend void ComponentsUI::ShowComponentTransformWindow(ComponentTransform *transform);
};

#endif //_COMPONENTTRANSFORM_H_