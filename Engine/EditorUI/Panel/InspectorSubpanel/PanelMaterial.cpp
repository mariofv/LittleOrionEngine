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
				bool is_selected = (material->material_type == ((Material::MaterialType)i));
				if (ImGui::Selectable(Material::GetMaterialTypeName((Material::MaterialType)i).c_str(), is_selected))
				{
					material->ChangeTypeOfMaterial((Material::MaterialType)i);
					const char* name = Material::GetMaterialTypeName(material->material_type).c_str();
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

bool PanelMaterial::ShowMaterialTextureMap(std::shared_ptr<Material> material, Material::MaterialTextureType type)
{
	
	ImGui::PushID(static_cast<unsigned int>(type));

	float material_texture_map_size = 20.F;

	void* display_image;
	if (material->textures[type].get() != nullptr)
	{
		if (type == Material::MaterialTextureType::LIQUID)
		{
			material->use_liquid_map = true;
		}
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

		if (ImGui::SliderFloat("Tiling X", &material->tiling_x, 0.f, 10.f))
		{
			modified_by_user = true;
		}
		if (ImGui::SliderFloat("Tiling Y", &material->tiling_y, 0.f, 10.f))
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

		if (material->material_type == Material::MaterialType::MATERIAL_LIQUID)
		{
			//TODO->change it to liquid maps and not hardcoded
			/*if (material->tiling_liquid_x_x >= 1)
			{
				material->tiling_liquid_x_x = 0;
				material->tiling_liquid_x_y = 0;
			}
			if (material->tiling_liquid_y_x <= -1)
			{
				material->tiling_liquid_y_x = 0;
				material->tiling_liquid_y_y = 0;
			}*/

			if (ImGui::SliderFloat("Speed Tiling X", &material->speed_tiling_x, 0.01f, 1.0f))
			{
				modified_by_user = true;
			}
			if (ImGui::SliderFloat("Speed Tiling Y", &material->speed_tiling_y, 0.01f, 1.0f))
			{
				modified_by_user = true;
			}
		}
	break;
	}



	if (ImGui::Button(ICON_FA_TIMES))
	{
		material->RemoveMaterialTexture(type);
		if (type == Material::MaterialTextureType::LIQUID)
		{
			material->use_liquid_map = false;
		}
		modified_by_user = true;
	}
	ImGui::SameLine();
	ImGui::Text("Remove Texture");
	ImGui::PopID();

	return modified_by_user;
}

std::string PanelMaterial::GetTypeName(Material::MaterialTextureType type)
{
	switch (type)
	{
	case  Material::MaterialTextureType::DIFFUSE:
		return "Difusse";
	case  Material::MaterialTextureType::SPECULAR:
		return "Specular";
	case  Material::MaterialTextureType::EMISSIVE:
		return "Emissive";
	case  Material::MaterialTextureType::OCCLUSION:
		return "Oclusion";
	case  Material::MaterialTextureType::NORMAL:
		return "Normal";
	case  Material::MaterialTextureType::LIGHTMAP:
		return "Lightmap";
	case  Material::MaterialTextureType::LIQUID:
		return "Liquid";

	default:
		return "";
	}
}