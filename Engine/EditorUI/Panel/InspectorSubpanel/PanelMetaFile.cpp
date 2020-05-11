#include "PanelMetaFile.h"

#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleFileSystem.h"

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

	ImGui::Text("TimeStamp:"); ImGui::SameLine();
	ImGui::InputScalar("###TimeStamp", ImGuiDataType_S64, &(metafile->timestamp), nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);
	ShowSpecializedMetaFile(metafile);

	ImGui::Separator();
	if (ImGui::Button("Apply"))
	{
		App->resources->metafile_manager->TouchMetafileTimestamp(*metafile);
		Path* imported_file_path = App->filesystem->GetPath(metafile->imported_file_path);
		App->resources->Import(*imported_file_path, true);
		App->resources->CleanResourceCache();
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