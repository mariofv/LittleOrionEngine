#include "PanelTrail.h"

#include "Component/ComponentTrail.h"
#include "EditorUI/Helper/ImGuiHelper.h"
#include "EditorUI/Panel/PanelPopups.h"
#include "EditorUI/Panel/PopupsPanel/PanelPopupResourceSelector.h"
#include "Module/ModuleActions.h"
#include "Module/ModuleEditor.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <FontAwesome5/IconsFontAwesome5.h>

PanelTrail::PanelTrail()
{
	enabled = true;
	opened = true;
	window_name = "Trail Inspector";
}

void PanelTrail::Render(ComponentTrail* trail)
{
	if (ImGui::CollapsingHeader(ICON_FA_SHARE " Trail Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Active", &trail->active);
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			App->actions->DeleteComponentUndo(trail);
			return;
		}
		if (ImGui::Button("Copy"))
		{
			App->actions->SetCopyComponent(trail);
		}
		if (ImGui::Button("Paste component as new"))
		{
			App->actions->PasteComponent(trail->owner);
		}
		if (ImGui::Button("Paste component values"))
		{
			App->actions->PasteComponentValues(trail);
		}
		ImGui::Separator();
		ImGui::TextColored(ImVec4(0.f, 0.f, 1.f, 1.f), "Trail Properties");
		trail->modified_by_user |= ImGui::DragFloat("Time", &trail->duration, 1000.0f);
		trail->modified_by_user |= ImGui::DragFloat("Width", &trail->width, 0.01f);
		trail->modified_by_user |= ImGui::DragInt("Curve Segments", &trail->points_in_curve, 1, 0, 100);
		
		float trail_texture_size = 17.5f;
		void* display_image;
		if (trail->trail_texture.get() != nullptr)
		{
			display_image = (void*)(intptr_t)trail->trail_texture->opengl_texture;
		}
		ImGuiID element_id = ImGui::GetID(std::to_string(trail->trail_texture->GetUUID()).c_str());
		if (ImGui::ImageButton(
			display_image,
			ImVec2(trail_texture_size, trail_texture_size),
			ImVec2(0, 1),
			ImVec2(1, 0),
			1,
			ImVec4(1.f, 1.f, 1.f, 1.f),
			ImVec4(1.f, 1.f, 1.f, 1.f)
		))
		{
			App->editor->popups->resource_selector_popup.ShowPanel(element_id, ResourceType::TEXTURE);
		}
		uint32_t dropped_texture_uuid = ImGui::ResourceDropper<Texture>();
		if (dropped_texture_uuid != 0)
		{
			trail->ChangeTexture(dropped_texture_uuid);
			trail->modified_by_user = true;
		}

		uint32_t resource_selector_texture = App->editor->popups->resource_selector_popup.GetSelectedResource(element_id);
		if (resource_selector_texture != 0)
		{
			trail->ChangeTexture(resource_selector_texture);
			trail->modified_by_user = true;
		}
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.f, 0.f, 1.f, 1.f), "Texture");

		int mode = static_cast<int>(trail->texture_mode);
		if (ImGui::Combo("TextureMode###Combo", &mode, "Stretch\0Tile\0RepeatPerSegment\0"))
		{
			switch (mode)
			{
			case 0:
				trail->texture_mode = ComponentTrail::TextureMode::STRETCH;
				break;
			case 1:
				trail->texture_mode = ComponentTrail::TextureMode::TILE;
				break;
			case 2:
				trail->texture_mode = ComponentTrail::TextureMode::REPEAT_PER_SEGMENT;
				break;
			}
		}
		if (trail->texture_mode == ComponentTrail::TextureMode::TILE)
		{
			trail->modified_by_user |= ImGui::DragInt("Tile Rows", &trail->rows, 1);
			trail->modified_by_user |= ImGui::DragInt("Tile Columns", &trail->columns, 1);
		}
		if (ImGui::CollapsingHeader("Color"))
		{
			ImGui::Spacing();
			trail->modified_by_user |= ImGui::ColorEdit4("Color", trail->color.ptr());
			trail->modified_by_user |= ImGui::DragFloat("Intensity", &trail->emissive_intensity, 0.05f, 0.01f, 10.0f);
			trail->modified_by_user |= ImGui::Checkbox("Blend Two Colors", &trail->blend_colors);
			if (trail->blend_colors)
			{
				trail->modified_by_user |= ImGui::ColorEdit4("Color Blend", trail->color_to_blend.ptr());
				trail->modified_by_user |= ImGui::DragFloat("Fraction Color 2", &trail->blend_percentage, 0.1f, 0.0f, 1.0F);
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.f, 0.f, 1.f, 1.f), "?");
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Width Percentage of Color 2 in relation to Color 1");
						
				trail->modified_by_user |= ImGui::DragFloat("Blending Width", &trail->smoothening_step, 0.01f, 0.1f, 1.0F);
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.f, 0.f, 1.f, 1.f), "?");
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Width Percentage where 2 colors are blended");
			}
		}
	}
}