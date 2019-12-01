#include "ComponentsUI.h"
#include "Component/ComponentMesh.h"
#include "Component/ComponentTransform.h"

#include <imgui.h>
#include <FontAwesome5/IconsFontAwesome5.h>

void ComponentsUI::ShowComponentTransformWindow(ComponentTransform *transform)
{
	if (ImGui::CollapsingHeader(ICON_FA_RULER_COMBINED " Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat3("Translation", &transform->translation[0], 0.01f))
		{
			transform->GenerateModelMatrix();
		}
		if (ImGui::DragFloat3("Rotation", &transform->rotation[0], 0.1f, -180.f, 180.f))
		{
			transform->GenerateModelMatrix();
		}
		if (ImGui::DragFloat3("Scale", &transform->scale[0], 0.01f))
		{
			transform->GenerateModelMatrix();
		}
	}
}

void ComponentsUI::ShowComponentMeshWindow(ComponentMesh *mesh)
{
	if (ImGui::CollapsingHeader(ICON_FA_SHAPES " Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Active", &mesh->active);
		ImGui::Separator();

		ImGui::DragInt("# Triangles", &mesh->num_triangles, NULL, NULL, NULL);
		ImGui::DragInt("# Vertices", &mesh->num_vertices, NULL, NULL, NULL);
	}
}