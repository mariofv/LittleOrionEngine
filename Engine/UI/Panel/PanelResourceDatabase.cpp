#include "PanelResourceDatabase.h"

#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"

#include <imgui.h>
#include <string>
#include <FontAwesome5/IconsFontAwesome5.h>

PanelResourceDatabase::PanelResourceDatabase()
{
	opened = false;
	enabled = true;
	window_name = ICON_FA_DATABASE " Resource Database";
}

void PanelResourceDatabase::Render()
{
	if (ImGui::Begin(ICON_FA_DATABASE " Resource Database", &opened))
	{
		hovered = ImGui::IsWindowHovered();

		ImGui::Columns(4, "ResourceDatabaseColumns"); // 4-ways, with border
		
		ImGui::Separator();
		ImGui::Text("UUID"); ImGui::NextColumn();
		ImGui::Text("Imported path"); ImGui::NextColumn();
		ImGui::Text("Exported path"); ImGui::NextColumn();
		ImGui::Text("Type"); ImGui::NextColumn();
		ImGui::Separator();

		static int selected = -1;
		int i = -1;
		for (auto& pair : App->resources->resource_DB.get()->entries)
		{
			++i;
			ImGui::PushID(pair.first);
			if (ImGui::Selectable(std::to_string(pair.first).c_str(), selected == i, ImGuiSelectableFlags_SpanAllColumns))
			{
				selected = i;
			}
			ImGui::NextColumn();
			ImGui::Text(pair.second->imported_file.c_str()); ImGui::NextColumn();
			ImGui::Text(pair.second->exported_file.c_str()); ImGui::NextColumn();
			ImGui::Text(GetResourceTypeName(pair.second->resource_type).c_str()); ImGui::NextColumn();
			ImGui::PopID();
		}
		ImGui::Columns(1);
	}
	ImGui::End();
}

std::string PanelResourceDatabase::GetResourceTypeName(ResourceType resource_type) const
{
	switch (resource_type)
	{
	case ResourceType::ANIMATION:
		return "Animation";
	case ResourceType::AUDIO:
		return "Audio";
	case ResourceType::MATERIAL:
		return "Material";
	case ResourceType::MESH:
		return "Mesh";
	case ResourceType::NAVMESH:
		return "NavMesh";
	case ResourceType::PREFAB:
		return "Prefab";
	case ResourceType::SCENE:
		return "Scene";
	case ResourceType::SKELETON:
		return "Skeleton";
	case ResourceType::SKYBOX:
		return "Skybox";
	case ResourceType::TEXTURE:
		return "Texture";
	case ResourceType::UNKNOWN:
		return "Unknown";
	}
}