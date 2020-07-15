#include "PanelMaterial.h"

#include "EditorUI/Helper/ImGuiHelper.h"
#include "EditorUI/Panel/PanelPopups.h"
#include "EditorUI/Panel/PopupsPanel/PanelPopupResourceSelector.h"
#include "EditorUI/Panel/PanelProjectExplorer.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleEditor.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleTexture.h"
#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Metafile/ModelMetafile.h"

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
	
	Metafile * metafile = App->resources->resource_DB->GetEntry(material->GetUUID());
	bool extracted = IsMaterialExtracted(material);

	bool is_core = material->IsCoreResource();

	if (is_core || extracted)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
	modified_by_user = false;

	if (ImGui::CollapsingHeader(ICON_FA_CIRCLE " Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Spacing();
		ImGui::Image((void *)App->texture->whitefall_texture_id, ImVec2(50, 50)); // TODO: Substitute this with resouce thumbnail
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		ImGui::Text(metafile->resource_name.c_str());
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

		if (ImGui::BeginCombo("Material Type",Material::GetMaterialTypeName(material->material_type).c_str()))
		{

			for (int i = 0; i < Material::MAX_MATERIAL_TYPES; ++i)
			{
				bool is_selected = (static_cast<int>(material->material_type) == i);
				if (ImGui::Selectable(Material::GetMaterialTypeName((Material::MaterialType)i).c_str(), is_selected))
				{
					material->ChangeTypeOfMaterial((Material::MaterialType)i);
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
		if (material->material_type == Material::MaterialType::MATERIAL_DISSOLVING || material->material_type == Material::MaterialType::MATERIAL_LIQUID)
		{
			ShowMaterialTextureMap(material, Material::MaterialTextureType::DISSOLVED_DIFFUSE);
			ShowMaterialTextureMap(material, Material::MaterialTextureType::DISSOLVED_EMISSIVE);
			ShowMaterialTextureMap(material, Material::MaterialTextureType::NOISE);
			ImGui::DragFloat("Dissolve Progress", &material->dissolve_progress, 0.01f, 0.f, 1.f);
		}
		ImGui::Spacing();

		ShowMaterialTextureMap(material, Material::MaterialTextureType::SPECULAR);
		ImGui::Spacing();

		ShowMaterialTextureMap(material, Material::MaterialTextureType::EMISSIVE);
		ImGui::Spacing();

		ShowMaterialTextureMap(material, Material::MaterialTextureType::OCCLUSION);
		ImGui::Spacing();

		ShowMaterialTextureMap(material, Material::MaterialTextureType::NORMAL);
		ImGui::Spacing();

		ShowMaterialTextureMap(material, Material::MaterialTextureType::LIGHTMAP);
		ImGui::Spacing();

		if (material->material_type == Material::MaterialType::MATERIAL_LIQUID)
		{
			ShowMaterialTextureMap(material, Material::MaterialTextureType::LIQUID);
			ImGui::Spacing();
		}
		

		ImGui::Separator();
	}

	if (modified_by_user)
	{
		App->resources->Save<Material>(material);
	}
	if (is_core || extracted)
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
}

bool PanelMaterial::IsMaterialExtracted(const std::shared_ptr<Material> &material)
{
	Metafile * meta = App->resources->resource_DB->GetEntry(material->GetUUID());
	return meta->version == 0;
}

void PanelMaterial::ShowMaterialTextureMap(std::shared_ptr<Material> material, Material::MaterialTextureType type)
{
	
	ImGui::PushID(static_cast<unsigned int>(type));

	float material_texture_map_size = 20.F;

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

		if (ImGui::ColorEdit3("Color", material->diffuse_color, ImGuiColorEditFlags_Float))
		{
			modified_by_user = true;
		}

		if (ImGui::DragFloat2("Tiling", material->tiling.ptr(), 0.1f, 0.f, 10.f))
		{
			modified_by_user = true;
		}
		if (material->material_type == Material::MaterialType::MATERIAL_TRANSPARENT || material->material_type == Material::MaterialType::MATERIAL_LIQUID )
		{
			if (ImGui::SliderFloat("Transparency", &material->transparency, 0.01f, 1.0f))
			{
				modified_by_user = true;
			}
		}

		ImGui::Unindent();

		break;

	case Material::MaterialTextureType::EMISSIVE:
		ImGui::Text("Emissive");

		ImGui::Spacing();
		ImGui::Indent();

		if (ImGui::ColorEdit3("Color", material->emissive_color))
		{
			modified_by_user = true;
		}
		ImGui::Unindent();

		break;

	case Material::MaterialTextureType::OCCLUSION:
		ImGui::Text("Occlusion");

		ImGui::Spacing();

		break;

	case Material::MaterialTextureType::SPECULAR:
		ImGui::Text("Specular");

		ImGui::Spacing();
		ImGui::Indent();
		
		if (ImGui::ColorEdit3("Color", material->specular_color, ImGuiColorEditFlags_Float))
		{
			modified_by_user = true;
		}
		if (ImGui::SliderFloat("Smoothenss", &material->smoothness, 0.f, 1.f))
		{
			modified_by_user = true;
		}

		ImGui::Unindent();

		break;

	case Material::MaterialTextureType::NORMAL:
		ImGui::Text("Normal");

		break;

	case Material::MaterialTextureType::LIGHTMAP:
		ImGui::Text("Lightmap");

		break;

	case Material::MaterialTextureType::LIQUID:
		ImGui::Text("Liquid");

		if (ImGui::DragFloat2("Liquid Tiling Speed", material->liquid_tiling_speed.ptr(), 0.1f, 0.f, 1.0f))
		{
			modified_by_user = true;
		}
		
		break;

	case Material::MaterialTextureType::DISSOLVED_DIFFUSE:
		ImGui::Text("Dissolved Diffuse");
		break;

	case Material::MaterialTextureType::DISSOLVED_EMISSIVE:
		ImGui::Text("Dissolved Emissive");
		break;

	case Material::MaterialTextureType::NOISE:
		ImGui::Text("Noise Map");
		break;
	}


	if (ImGui::Button(ICON_FA_TIMES))
	{
		material->RemoveMaterialTexture(type);
		modified_by_user = true;
	}
	ImGui::SameLine();
	ImGui::Text("Remove Texture");
	ImGui::PopID();
}