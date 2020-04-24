#include "PanelMetaFile.h"

#include "ResourceManagement/Metafile/Metafile.h"

#include <imgui.h>
#include <imgui_stdlib.h>

PanelMetaFile::PanelMetaFile()
{
	enabled = true;
	opened = true;
	window_name = "Importing Options Inspector";
}

void PanelMetaFile::Render(Metafile * meta_file)
{
	ImGui::Text("Resource:"); ImGui::SameLine();
	ImGui::Text(meta_file->resource_name.c_str());

	ImGui::Text("UUID"); ImGui::SameLine();
	ImGui::InputScalar("###UUID", ImGuiDataType_U32, &(meta_file->uuid), nullptr,nullptr,nullptr,ImGuiInputTextFlags_ReadOnly);

	ImGui::Text("Exported path:"); ImGui::SameLine();
	ImGui::Text(meta_file->exported_file_path.c_str());

	ImGui::Text("Imported path:"); ImGui::SameLine();
	ImGui::Text(meta_file->imported_file_path.c_str());

	ImGui::Text("TimeStamp:"); ImGui::SameLine();
	ImGui::InputScalar("###TimeStamp", ImGuiDataType_S64, &(meta_file->timestamp), nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);
}
