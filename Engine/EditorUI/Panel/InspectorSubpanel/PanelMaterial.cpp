#include "PanelMaterial.h"

#include "EditorUI/Helper/ImGuiHelper.h"
#include "EditorUI/Panel/PanelPopups.h"
#include "EditorUI/Panel/PopupsPanel/PanelPopupResourceSelector.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleEditor.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleTexture.h"
#include "Module/ModuleResourceManager.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <FontAwesome5/IconsFontAwesome5.h>

PanelMaterial::PanelMaterial()
{
	enabled = true;
	opened = true;
	window_name = "Material Inspector";
}

void PanelMaterial::Render(std::shared_ptr<Material> material)
{
	if (material->IsCoreResource())
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
	modified_by_user = false;

	if (ImGui::CollapsingHeader(ICON_FA_IMAGE " Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Spacing();
		ImGui::Image((void *)App->texture->whitefall_texture_id, ImVec2(50, 50)); // TODO: Substitute this with resouce thumbnail
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		ImGui::Text(App->resources->resource_DB->GetEntry(material->GetUUID())->resource_name.c_str());
		ImGui::Spacing();

		if (ImGui::BeginCombo("Shader", material->shader_program.c_str()))
		{
			for (auto& program : App->program->names)
			{
				bool is_selected = (material->shader_program == program);
				if (ImGui::Selectable(program, is_selected))
				{
					material->shader_program = program;
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
					modified_by_user = true;
				}

			}
			ImGui::EndCombo();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Main Maps");
		ImGui::Spacing();
		ImGui::Spacing();

		ShowMaterialTextureMap(material, Material::MaterialTextureType::DIFFUSE);
		ImGui::Spacing();

		ShowMaterialTextureMap(material, Material::MaterialTextureType::SPECULAR);
		ImGui::Spacing();

		ShowMaterialTextureMap(material, Material::MaterialTextureType::EMISSIVE);
		ImGui::Spacing();

		ShowMaterialTextureMap(material, Material::MaterialTextureType::OCCLUSION);
		ImGui::Spacing();

		ImGui::Separator();
	}

	if (modified_by_user)
	{
		App->resources->Save<Material>(material);
	}
	if (material->IsCoreResource())
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
}

void PanelMaterial::ShowMaterialTextureMap(std::shared_ptr<Material> material, Material::MaterialTextureType type)
{
	ImGui::PushID(static_cast<unsigned int>(type));

	float material_texture_map_size = 20.f;

	void* display_image;
	if (material->textures[type].get() != nullptr)
	{
		std::shared_ptr<Texture>& texture = material->textures[type];
		display_image = (void*)(intptr_t)texture->opengl_texture;
	}
	else
	{
		display_image = (void*)0;
	}

	ImGuiID element_id = ImGui::GetID(std::to_string(material->GetUUID() + type).c_str());
	if (ImGui::ImageButton(
		display_image,
		ImVec2(material_texture_map_size, material_texture_map_size),
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
		material->SetMaterialTexture(type, dropped_texture_uuid);
		modified_by_user = true;
	}

	uint32_t resource_selector_texture = App->editor->popups->resource_selector_popup.GetSelectedResource(element_id);
	if (resource_selector_texture != 0)
	{
		material->SetMaterialTexture(type, resource_selector_texture);
		modified_by_user = true;
	}

	ImGui::SameLine();
	ImGui::AlignTextToFramePadding();

	switch (type)
	{
	case Material::MaterialTextureType::DIFFUSE:
		ImGui::Text("Diffuse");

		ImGui::Spacing();
		ImGui::Indent();

		ImGui::ColorEdit3("Color", material->diffuse_color);
		ImGui::SliderFloat("K diffuse", &material->k_diffuse, 0, 1);
		ImGui::Unindent();

		break;

	case Material::MaterialTextureType::EMISSIVE:
		ImGui::Text("Emissive");

		ImGui::Spacing();
		ImGui::Indent();

		ImGui::ColorEdit3("Color", material->emissive_color);
		ImGui::Unindent();

		break;

	case Material::MaterialTextureType::OCCLUSION:
		ImGui::Text("Occlusion");

		ImGui::Spacing();
		ImGui::Indent();

		ImGui::SliderFloat("k ambient", &material->k_ambient, 0, 1);
		ImGui::Unindent();

		break;

	case Material::MaterialTextureType::SPECULAR:
		ImGui::Text("Specular");

		ImGui::Spacing();
		ImGui::Indent();

		ImGui::ColorEdit3("Color", material->specular_color);
		ImGui::SliderFloat("k specular", &material->k_specular, 0, 1);
		ImGui::Unindent();

		break;
	}

	ImGui::PopID();
}

std::string PanelMaterial::GetTypeName(Material::MaterialTextureType type)
{
	switch (type)
	{
	case  Material::MaterialTextureType::DIFFUSE:
		return "Difusse";
		break;
	case  Material::MaterialTextureType::SPECULAR:
		return "Specular";
		break;
	case  Material::MaterialTextureType::EMISSIVE:
		return "Emissive";
		break;
	case  Material::MaterialTextureType::OCCLUSION:
		return "Oclusion";
		break;

	default:
		return "";
	}
}