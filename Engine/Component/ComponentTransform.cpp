#include "ComponentTransform.h"

ComponentTransform::ComponentTransform()
{
	GenerateModelMatrix();
}

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

Component::ComponentType ComponentTransform::GetType() const
{
	return Component::ComponentType::TRANSFORM;
}

void ComponentTransform::Render(const GLuint shader_program) const
{
	float4x4 tmp_model_matrix = model_matrix;

	glUniformMatrix4fv(
		glGetUniformLocation(shader_program, "model"),
		1,
		GL_TRUE,
		&tmp_model_matrix[0][0]
	);
}

void ComponentTransform::GenerateModelMatrix()
{
	model_matrix = float4x4::FromTRS(translation, rotation, scale);
}