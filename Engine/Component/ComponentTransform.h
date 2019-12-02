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
	ComponentTransform(GameObject * owner,const float3 translation, const float3 rotation, const float3 scale);

	~ComponentTransform() = default;

	void Enable() override;
	void Disable() override;
	void Update() override;

	void Render(const GLuint shader_program) const;
	
	float3 GetTranslation() const;
	void SetTranslation(const float3 translation);

	void GenerateGlobalModelMatrix();
	void ChangeLocalSpace(const float4x4 new_local_space);

	float4x4 GetGlobalModelMatrix() const;

	void ShowComponentWindow() override;

private:
	void GenerateModelMatrix();

private:
	float3 translation = float3::zero;
	float3 rotation = float3::zero;
	float3 scale = float3::one;

	float4x4 model_matrix = float4x4::identity;
	float4x4 global_model_matrix = float4x4::identity;

	friend void ComponentsUI::ShowComponentTransformWindow(ComponentTransform *transform);
};

#endif //_COMPONENTTRANSFORM_H_