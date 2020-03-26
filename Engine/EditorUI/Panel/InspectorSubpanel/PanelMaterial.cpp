#include "PanelMaterial.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleFileSystem.h"
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
		ImGui::Text(material->exported_file.c_str());
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
					SaveMaterial(material);
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

	if (material->textures[type].get() != nullptr) {
		char tmp_string[256];
		std::shared_ptr<Texture>& texture = material->textures[type];
		ImGui::Image(
			(void*)(intptr_t)texture->opengl_texture,
			ImVec2(material_texture_map_size, material_texture_map_size),
			ImVec2(0, 1),
			ImVec2(1, 0),
			ImVec4(1.f, 1.f, 1.f, 1.f),
			ImVec4(1.f, 1.f, 1.f, 1.f)
		);
		DropTarget(material, type);
	}
	else
	{
		ImGui::Image(
			(void*)0,
			ImVec2(material_texture_map_size, material_texture_map_size),
			ImVec2(0, 1),
			ImVec2(1, 0),
			ImVec4(1.f, 1.f, 1.f, 1.f),
			ImVec4(1.f, 1.f, 1.f, 1.f)
		);
		DropTarget(material, type);
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
		ImGui::SliderFloat("Shininess", &material->shininess, 0, 1);
		ImGui::SliderFloat("Roughness", &material->roughness, 0, 100);
		ImGui::SliderFloat("Metalness", &material->metalness, 0, 100);
		ImGui::Unindent();

		break;
	}

	ImGui::PopID();
}

void PanelMaterial::SaveMaterial(Material* material)
{
	Config material_config;
	material->Save(material_config);

	std::string serialized_material_string;
	material_config.GetSerializedString(serialized_material_string);
	//Remove this when using uuid completely
	if (std::find_if(material->exported_file.begin(), material->exported_file.end(), ::isdigit) == material->exported_file.end())
	{
		return;
	}
	std::string uid_string = material->exported_file.substr(material->exported_file.find_last_of("/") + 1, material->exported_file.size());
	uint32_t real_uuid = std::stoul(uid_string);

	std::string imported_file = App->resources->resource_DB->GetEntry(real_uuid)->imported_file;
	App->filesystem->Save(imported_file.c_str(), serialized_material_string.c_str(), serialized_material_string.size() + 1);
	App->filesystem->Save(material->exported_file.c_str(), serialized_material_string.c_str(), serialized_material_string.size() + 1);
}

void PanelMaterial::DropTarget(Material* material, Material::MaterialTextureType type)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("DND_File"))
		{
			assert(payload->DataSize == sizeof(File*));
			File* incoming_file = *(File * *)payload->Data;
			if (incoming_file->file_type == FileType::TEXTURE)
			{
				/*
				//UndoRedo
				App->actions->type_texture = type;
				App->actions->action_component = material;
				App->actions->AddUndoAction(ModuleActions::UndoActionType::EDIT_COMPONENTMATERIAL);
				*/
				material->SetMaterialTexture(type, App->texture->LoadTexture(incoming_file->file_path.c_str()));
				SaveMaterial(material);
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