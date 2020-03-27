#include "PanelProjectExplorer.h"

#include "Filesystem/PathAtlas.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTexture.h"
#include "ResourceManagement/Manager/PrefabManager.h"
#include "ResourceManagement/Manager/SkyboxManager.h"
#include "ResourceManagement/Metafile/Metafile.h"
#include "ResourceManagement/Metafile/MetafileManager.h"
#include "ResourceManagement/Resources/Prefab.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <SDL/SDL.h>
#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>
#include <algorithm>

PanelProjectExplorer::PanelProjectExplorer()
{
	opened = true;
	enabled = true;
	window_name = ICON_FA_FOLDER " Project Explorer";
}

void PanelProjectExplorer::Render()
{
	if (ImGui::Begin(ICON_FA_FOLDER " Project Explorer", &opened))
	{
		hovered = ImGui::IsWindowHovered();
		focused = ImGui::IsWindowFocused();

		project_explorer_dockspace_id = ImGui::GetID("ProjectExplorerDockspace");
		bool initialized = ImGui::DockBuilderGetNode(project_explorer_dockspace_id) != NULL;

		ImGuiDockNodeFlags resources_explorer_dockspace_flags = ImGuiDockNodeFlags_None;
		resources_explorer_dockspace_flags |= ImGuiDockNodeFlags_NoWindowMenuButton;
		resources_explorer_dockspace_flags |= ImGuiDockNodeFlags_NoCloseButton;

		ImGui::DockSpace(project_explorer_dockspace_id, ImVec2(0, 0), resources_explorer_dockspace_flags);

		if (!initialized)
		{
			InitResourceExplorerDockspace();
		}

		if (ImGui::Begin("Project Folder Explorer"))
		{
			ShowFileSystemActionsMenu(selected_folder);
			ShowFoldersHierarchy(*App->filesystem->assets_folder_path);
		}
		ImGui::End();

		ImGui::SameLine();
		if (ImGui::Begin("Project File Explorer")) 
		{
			ImGui::BeginChild("Project File Explorer Drop Target");
			ShowFileSystemActionsMenu(selected_folder);
			ShowFilesInExplorer();
			ImGui::EndChild();
			FilesDrop();
		}
		ImGui::End();
	}
	ImGui::End();
}

void PanelProjectExplorer::InitResourceExplorerDockspace()
{
	ImGui::DockBuilderRemoveNode(project_explorer_dockspace_id); // Clear out existing layout
	ImGui::DockBuilderAddNode(project_explorer_dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
	ImVec2 child_window_size = ImGui::GetWindowSize();
	ImGui::DockBuilderSetNodeSize(project_explorer_dockspace_id, child_window_size);

	ImGuiID dock_main_id = project_explorer_dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.

	ImGuiID dock_id_left;
	ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.80f, NULL, &dock_id_left);


	ImGui::DockBuilderDockWindow("Project Folder Explorer", dock_id_left);
	ImGui::DockBuilderDockWindow("Project File Explorer", dock_id_right);

	ImGuiDockNode* left_node = ImGui::DockBuilderGetNode(dock_id_left);
	left_node->LocalFlags = ImGuiDockNodeFlags_NoTabBar;

	ImGuiDockNode* right_node = ImGui::DockBuilderGetNode(dock_id_right);
	right_node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

	ImGui::DockBuilderFinish(project_explorer_dockspace_id);
}

void PanelProjectExplorer::ShowFoldersHierarchy(const Path& path)
{
	for (auto & path_child : path.children)
	{
		if (path_child->IsDirectory())
		{

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

			std::string filename = ICON_FA_FOLDER " " + path_child->GetFilename();
			if (path_child->sub_folders == 0)
			{
				flags |= ImGuiTreeNodeFlags_Leaf;
			}
			if (selected_folder == path_child)
			{
				flags |= ImGuiTreeNodeFlags_Selected;
				filename = ICON_FA_FOLDER_OPEN " " + path_child->GetFilename();
			}
			bool expanded = ImGui::TreeNodeEx(filename.c_str(), flags);
			if (expanded) {
				ImGui::PushID(filename.c_str());
				ProcessMouseInput(path_child);
				ShowFoldersHierarchy(*path_child);
				ImGui::PopID();
				ImGui::TreePop();
			}
		}
	}
}

void PanelProjectExplorer::ShowFilesInExplorer()
{
	if (selected_folder == nullptr)
	{
		return;
	}

	ImVec2 available_region = ImGui::GetContentRegionAvail();
	int files_per_line = available_region.x / file_size_width;

	int current_line = 0;
	int current_file_in_line = 0;

	for (auto & child_path : selected_folder->children)
	{
		if (child_path->IsMeta())
		{
			ImGui::PushID(current_line * files_per_line + current_file_in_line);
			ShowMetafileIcon(child_path);
			ImGui::PopID();

			++current_file_in_line;
			if (current_file_in_line == files_per_line)
			{
				current_file_in_line = 0;
				++current_line;
			}
			else
			{
				ImGui::SameLine();
			}
		}
	}
}

void PanelProjectExplorer::ShowMetafileIcon(Path* metafile_path)
{
	Metafile* metafile = App->resources->metafile_manager->GetMetafile(*metafile_path);

	std::string filename = metafile_path->GetFilenameWithoutExtension();

	if (ImGui::BeginChild(filename.c_str(), ImVec2(file_size_width, file_size_height), selected_file == metafile_path, ImGuiWindowFlags_NoDecoration))
	{
		ResourceDragSource(metafile);
		ProcessResourceMouseInput(metafile_path);

		ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 0.75 * file_size_width) * 0.5f);
		ImGui::Image((void *)App->texture->whitefall_texture_id, ImVec2(0.75*file_size_width, 0.75*file_size_width)); // TODO: Substitute this with resouce thumbnail
		ImGui::Spacing();

		float text_width = ImGui::CalcTextSize(filename.c_str()).x;
		if (text_width < file_size_width)
		{
			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_width) * 0.5f);
			ImGui::Text(filename.c_str());
		}
		else
		{
			int character_width = text_width / filename.length();
			int string_position_wrap = file_size_width / character_width - 5;
			assert(string_position_wrap < filename.length());
			std::string wrapped_filename = filename.substr(0, string_position_wrap+3) + "\n" + filename.substr(string_position_wrap, filename.size());
			ImGui::Text(wrapped_filename.c_str());
		}
	}
	ImGui::EndChild();
}

void PanelProjectExplorer::ResourceDragSource(Metafile* metafile) const
{
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("DND_Resource", &metafile, sizeof(Metafile*));
		ImGui::Text("Dragging %s", metafile->imported_file_path.c_str());
		ImGui::EndDragDropSource();
	}
}

void PanelProjectExplorer::ProcessResourceMouseInput(Path* file)
{
	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
	{
		selected_file = file;
	}
}

void PanelProjectExplorer::ProcessMouseInput(Path* file)
{
	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			selected_folder = file;
			selected_file = nullptr;
		}
		else if (ImGui::IsMouseDoubleClicked(0))
		{
			selected_folder = file;
		}
	}
}

void PanelProjectExplorer::ShowFileSystemActionsMenu(Path* path)
{
	if (path == nullptr)
	{
		return;
	}
	std::string label("Menu");
	if (ImGui::BeginPopupContextWindow(label.c_str()))
	{
		
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::Selectable("Material"))
			{
				MaterialManager::Create(*path);
			}
			if (ImGui::Selectable("Skybox"))
			{
				SkyboxManager ::Create(*path);
			}
			ImGui::EndMenu();
		}
		/* TODO: Finish this
		if (ImGui::Selectable("Delete"))
		{
			bool success = App->filesystem->Remove(file);
			if (success)
			{
				selected_file = nullptr;
				changes = true;
			}
		}
		if (file->file_type == FileType::MODEL && ImGui::Selectable("Extract Prefab"))
		{
			std::string new_prefab_name = file->GetFullPath().substr(0, file->GetFullPath().find_last_of(".")) + ".prefab";
			ImportOptions options;
			Importer::GetOptionsFromMeta(Importer::GetMetaFilePath(file->GetFullPath()), options);
			App->filesystem->Copy(options.exported_file.c_str(), new_prefab_name.c_str());
			App->resources->Import(new_prefab_name);
			changes = true;

		}
		if (ImGui::Selectable("Reimport"))
		{
			App->resources->ImportAllFilesInDirectory(*selected_folder, true);
			changes = true;
		}
		if (ImGui::Selectable("Reimport All"))
		{
			App->resources->ImportAllFilesInDirectory(*App->filesystem->assets_file, true);
			changes = true;
		}
		if (changes)
		{
			selected_folder->Refresh();
		}
		/*if (ImGui::Selectable("Rename"))
		{
			//TODO
		}
		if (ImGui::Selectable("Copy"))
		{
			//TODO
		}*/

		ImGui::EndPopup();
	}
}

void PanelProjectExplorer::FilesDrop() const
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_GameObject"))
		{
			assert(payload->DataSize == sizeof(GameObject*));
			GameObject *incoming_game_object = *(GameObject**)payload->Data;
			if (incoming_game_object->prefab_reference == nullptr)
			{
				uint32_t prefab_uuid = PrefabManager::CreateFromGameObject(*selected_folder, *incoming_game_object);
				if (prefab_uuid != -1)
				{
					App->scene->RemoveGameObject(incoming_game_object);
					std::shared_ptr<Prefab> prefab = std::static_pointer_cast<Prefab>(App->resources->Load(prefab_uuid));
					App->editor->selected_game_object = prefab->Instantiate(App->scene->GetRoot());
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
}