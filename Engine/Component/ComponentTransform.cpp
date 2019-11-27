#include "ComponentTransform.h"
#include "GameObject.h"

#include "imgui.h"
#include "IconsFontAwesome5.h"

ComponentTransform::ComponentTransform()
{
	GenerateModelMatrix();
}

ComponentTransform::ComponentTransform(const float3 translation, const float3 rotation, const float3 scale) :
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

Component::ComponentType ComponentTransform::GetType() const
{
	return Component::ComponentType::TRANSFORM;
}

void ComponentTransform::Render(const GLuint shader_program) const
{
	float4x4 tmp_model_matrix = global_model_matrix;

	glUniformMatrix4fv(
		glGetUniformLocation(shader_program, "model"),
		1,
		GL_TRUE,
		&tmp_model_matrix[0][0]
	);
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
		global_model_matrix = owner->parent->transform->global_model_matrix * model_matrix;
	}
}

void ComponentTransform::ShowComponentWindow()
{
	if (ImGui::CollapsingHeader(ICON_FA_RULER_COMBINED " Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat3("Translation", &translation[0], 0.01f))
		{
			GenerateModelMatrix();
		}
		if (ImGui::DragFloat3("Rotation", &rotation[0], 0.1f, -180.f, 180.f))
		{
			GenerateModelMatrix();
		}
		if (ImGui::DragFloat3("Scale", &scale[0], 0.01f))
		{
			GenerateModelMatrix();
		}
	}
}