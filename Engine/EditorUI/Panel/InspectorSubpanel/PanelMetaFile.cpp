#include "PanelMetaFile.h"

#include "EditorUI/Helper/ImGuiHelper.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleScene.h"

#include "ResourceManagement/Metafile/Metafile.h"
#include "ResourceManagement/Metafile/ModelMetafile.h"
#include "ResourceManagement/Metafile/TextureMetafile.h"
#include "Helper/Config.h"

#include <imgui.h>
#include <imgui_stdlib.h>

PanelMetaFile::PanelMetaFile()
{
	enabled = true;
	opened = true;
	window_name = "Importing Options Inspector";
}

void PanelMetaFile::Render(Metafile * metafile)
{
	ImGui::Text("Resource:"); ImGui::SameLine();
	ImGui::Text(metafile->resource_name.c_str());

	ImGui::Text("UUID"); ImGui::SameLine();
	ImGui::InputScalar("###UUID", ImGuiDataType_U32, &(metafile->uuid), nullptr,nullptr,nullptr,ImGuiInputTextFlags_ReadOnly);

	ImGui::Text("Exported path:"); ImGui::SameLine();
	ImGui::Text(metafile->exported_file_path.c_str());

	ImGui::Text("Imported path:"); ImGui::SameLine();
	ImGui::Text(metafile->imported_file_path.c_str());

	ShowSpecializedMetaFile(metafile);

	ImGui::Separator();
	if (ImGui::Button("Apply"))
	{
		ApplyMetafileChanges(metafile);
	}
}

void PanelMetaFile::ApplyMetafileChanges(Metafile * metafile)
{
	App->resources->metafile_manager->UpdateMetafile(*metafile);
	Path* imported_file_path = App->filesystem->GetPath(metafile->imported_file_path);
	App->resources->Import(*imported_file_path, true);

	bool found = App->resources->CleanResourceFromCache(metafile->uuid);
	if (found)
	{
		App->scene->SaveTmpScene();
		App->scene->LoadTmpScene();
	}
}

void PanelMetaFile::ShowSpecializedMetaFile(Metafile * metafile)
{
	ImGui::Separator();
	switch (metafile->resource_type)
	{
	case ResourceType::MODEL:
		ShowModelMetaFile(static_cast<ModelMetafile*>(metafile));
		break;
	case ResourceType::TEXTURE:
		ShowTextureMetaFile(static_cast<TextureMetafile*>(metafile));
		break;
	}
}

void PanelMetaFile::ShowModelMetaFile(ModelMetafile * metafile)
{
	ImGui::Checkbox("Apply scale", &metafile->convert_units);
	if (metafile->convert_units)
	{
		ImGui::Text("Scale Factor"); ImGui::SameLine();
		ImGui::InputFloat("###Scale", &(metafile->scale_factor));
	}

	ImGui::Spacing();
	ImGui::Checkbox("Import Meshes",&metafile->import_mesh);
	ImGui::Checkbox("Import Animations", &metafile->import_animation);
	ImGui::Checkbox("Import Rigging", &metafile->import_rig);
	if (metafile->import_rig)
	{
		ImGui::SameLine();
		ImGui::Checkbox("Complex Rigging", &metafile->complex_skeleton);
	}
	ImGui::Checkbox("Import Materials", &metafile->import_material);
	if (metafile->import_material)
	{
		ImGui::Text("Remapped Materials");
		for (auto & pair : metafile->remapped_materials)
		{
			ImGui::Text(pair.first.c_str());
			ImGui::SameLine();
			ImGui::Button(std::to_string(pair.second).c_str());
			uint32_t incoming_material_uuid = ImGui::ResourceDropper<Material>();
			if (incoming_material_uuid != 0)
			{
				metafile->remapped_materials[pair.first] = incoming_material_uuid;
			}

		}
	}
}

void PanelMetaFile::ShowTextureMetaFile(TextureMetafile * metafile)
{
	static std::vector<const char*> texture_types = {"Default", "Normal Map"};
	if (ImGui::BeginCombo("Texture type", GetTextureTypeName(metafile->texture_options.texture_type).c_str()))
	{
		size_t i = 0;
		for (auto& texture_type : texture_types)
		{
			bool is_selected = (i == metafile->texture_options.texture_type);
			if (ImGui::Selectable(texture_type, is_selected))
			{
				metafile->texture_options.texture_type = static_cast<TextureType>(i);
			}
			++i;
		}
		ImGui::EndCombo();
	}

	static std::vector<const char*> texture_wrap_mode = { "Repeat","Clamp", "Mirror","Mirror once"};
	if (ImGui::BeginCombo("Wrap mode", GetTextureWrapModeName(metafile->texture_options.wrap_mode).c_str()))
	{
		size_t i = 0;
		for (auto& wrap_mode : texture_wrap_mode)
		{
			bool is_selected = (i == metafile->texture_options.wrap_mode);
			if (ImGui::Selectable(wrap_mode, is_selected))
			{
				metafile->texture_options.wrap_mode = static_cast<WrapMode>(i);
			}
			++i;
		}
		ImGui::EndCombo();
	}

	static std::vector<const char*> texture_filter_modes = { "Nearest", "Linear" };
	if (ImGui::BeginCombo("Filter mode", GetTextureFilterModeName(metafile->texture_options.filter_mode).c_str()))
	{
		size_t i = 0;
		for (auto& filter_mode : texture_filter_modes)
		{
			bool is_selected = (i == metafile->texture_options.filter_mode);
			if (ImGui::Selectable(filter_mode, is_selected))
			{
				metafile->texture_options.filter_mode = static_cast<FilterMode>(i);
			}
			++i;
		}
		ImGui::EndCombo();
	}
	ImGui::Checkbox("Generate Mip Maps", &metafile->texture_options.generate_mipmaps);
}

std::string PanelMetaFile::GetTextureTypeName(TextureType texture_type_id) const
{
	switch (texture_type_id)
	{
	case TextureType::DEFAULT:
		return "Default";
		break;
	case TextureType::NORMAL:
		return "Normal Map";
		break;
	default:
		return "Default";
		break;
	}
	return "";
}

std::string PanelMetaFile::GetTextureFilterModeName(FilterMode filter_mode) const
{
	switch (filter_mode)
	{
	case NEAREST:
		return "Nearest";
		break;
	case LINEAR:
		return "Linear";
	default:
		break;
	}

}

std::string PanelMetaFile::GetTextureWrapModeName(WrapMode wrap_mode) const
{
	switch (wrap_mode)
	{
	case REPEAT:
		return "Repeat";
		break;
	case CLAMP:
		return "Clamp";
		break;
	case MIRROR:
		return "Mirror";
		break;
	case MIRROR_ONCE:
		return "Mirror once";
		break;
	default:
		return "Repeat";
		break;
	}
}