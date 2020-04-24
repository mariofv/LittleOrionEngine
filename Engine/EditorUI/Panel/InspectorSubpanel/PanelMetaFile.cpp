#include "PanelMetaFile.h"

#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleFileSystem.h"

#include "ResourceManagement/Metafile/Metafile.h"
#include "ResourceManagement/Metafile/ModelMetafile.h"
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

	ImGui::Spacing();
	if (ImGui::Button("Apply"))
	{
		App->resources->metafile_manager->TouchMetafileTimestamp(*metafile);
		Path* imported_file_path = App->filesystem->GetPath(metafile->imported_file_path);
		App->resources->Import(*imported_file_path);
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
	default:
		break;
	}
}

void PanelMetaFile::ShowModelMetaFile(ModelMetafile * metafile)
{
	ImGui::Text("Scale Factor"); ImGui::SameLine();
	ImGui::InputFloat("###Scale",&(metafile->scale_factor));
	ImGui::Checkbox("Apply scale", &metafile->convert_units);

	ImGui::Spacing();
	ImGui::Checkbox("Import Meshes",&metafile->import_mesh);
	ImGui::Checkbox("Import Animations", &metafile->import_animation);
	ImGui::Checkbox("Import Rigging", &metafile->import_rig);
	ImGui::Checkbox("Import Materials", &metafile->import_material);
}

