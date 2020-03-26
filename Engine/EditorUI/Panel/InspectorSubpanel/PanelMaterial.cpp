#include "PanelMaterial.h"

#include "EditorUI/Panel/PanelPopups.h"
#include "EditorUI/Panel/PopupsPanel/PanelPopupResourceSelector.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleTexture.h"
#include "Module/ModuleResourceManager.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <FontAwesome5/IconsFontAwesome5.h>

PanelMaterial::PanelMaterial()
{
	enabled = true;
	opened = true;
	window_name = "Material Inspector";
}

void PanelMaterial::Render(Material* material)
{
	if (ImGui::CollapsingHeader(ICON_FA_IMAGE " Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Spacing();
		ImGui::Image((void *)App->texture->whitefall_texture_id, ImVec2(50, 50)); // TODO: Substitute this with resouce thumbnail
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		ImGui::Text(material->resource_metafile->imported_file_path.c_str());
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
					//SaveMaterial(material);
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
}

void PanelMaterial::ShowMaterialTextureMap(Material* material, Material::MaterialTextureType type)
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
	DropTarget(material, type);

	std::shared_ptr<Resource> resource_selector_texture;
	App->editor->popups->resource_selector_popup.GetSelectedResource(element_id, resource_selector_texture);
	if (resource_selector_texture != nullptr)
	{
		material->SetMaterialTexture(type, std::static_pointer_cast<Texture>(resource_selector_texture));
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

void PanelMaterial::DropTarget(Material* material, Material::MaterialTextureType type)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("DND_Resource"))
		{
			assert(payload->DataSize == sizeof(Metafile*));
			Metafile* incoming_metafile = *((Metafile**)payload->Data);
			if (incoming_metafile->resource_type == ResourceType::TEXTURE)
			{
				/*
				//UndoRedo
				App->actions->type_texture = type;
				App->actions->action_component = material;
				App->actions->AddUndoAction(ModuleActions::UndoActionType::EDIT_COMPONENTMATERIAL);
				*/
				std::shared_ptr<Texture> texture = std::static_pointer_cast<Texture>(App->resources->Load(incoming_metafile->uuid));
				material->SetMaterialTexture(type, texture);
			}
		}
		ImGui::EndDragDropTarget();
	}
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