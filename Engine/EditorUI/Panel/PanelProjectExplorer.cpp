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
#include "ResourceManagement/Resources/StateMachine.h"

#include "PanelStateMachine.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <SDL/SDL.h>
#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>
#include <algorithm>

static std::string new_name_file;
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
			hovered =  ImGui::IsWindowHovered();
			ShowFoldersHierarchy(*App->filesystem->assets_folder_path);
		}
		ImGui::End();

		ImGui::SameLine();
		if (ImGui::Begin("Project File Explorer")) 
		{
			ImGui::BeginChild("Project File Explorer Drop Target");
			hovered = ImGui::IsWindowHovered() ? true : hovered;
			ShowFilesInExplorer();
			ImGui::EndChild();
			FilesDrop();
		}
		ImGui::End();

		ShowFileSystemActionsMenu(selected_folder);
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
			ResourceDropTarget(path_child);
			ImGui::PushID(filename.c_str());
			ProcessMouseInput(path_child);
			if (expanded)
			{
				ShowFoldersHierarchy(*path_child);
				ImGui::TreePop();
			}
			ImGui::PopID();
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
		if (child_path != nullptr && child_path->IsMeta())
		{
			ImGui::PushID(current_line * files_per_line + current_file_in_line);
			ShowMetafile(child_path);
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

void PanelProjectExplorer::ShowMetafile(Path* metafile_path)
{
	Metafile* metafile = App->resources->metafile_manager->GetMetafile(*metafile_path);

	std::string filename = metafile_path->GetFilenameWithoutExtension();

	if (ImGui::BeginChild(filename.c_str(), ImVec2(file_size_width, file_size_height), selected_file == metafile_path, ImGuiWindowFlags_NoDecoration))
	{
		hovered = ImGui::IsWindowHovered() ? true : hovered;

		ResourceDragSource(metafile);
		ProcessResourceMouseInput(metafile_path, metafile);

		ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 0.75 * file_size_width) * 0.5f);
		ShowMetafileIcon(metafile);
		ImGui::Spacing();

		if (renaming_file && metafile_path == renaming_file)
		{
			ImGui::InputText("###NewName", &new_name_file);
		}
		else
		{
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
				std::string wrapped_filename = filename.substr(0, string_position_wrap + 3) + "\n" + filename.substr(string_position_wrap, filename.size());
				ImGui::Text(wrapped_filename.c_str());
			}
		}
	}
	ImGui::EndChild();
}

void PanelProjectExplorer::ShowMetafileIcon(Metafile * metafile)
{

	ImGui::SetWindowFontScale(2);

	if(metafile->resource_type == ResourceType::TEXTURE)
	{
		ImGui::Image((void *)GetResourcePreviewImage(metafile->uuid), ImVec2(0.75*file_size_width, 0.75*file_size_width));
	}
	else
	{
		std::string icon;
		switch (metafile->resource_type)
		{
		case ResourceType::ANIMATION:
			icon = ICON_FA_PLAY_CIRCLE;
			break;
		case ResourceType::MODEL:
			icon = ICON_FA_BOX;
			break;
		case ResourceType::MESH:
			icon = ICON_FA_DRAW_POLYGON;
			break;
		case ResourceType::PREFAB:
			icon = ICON_FA_BOX_OPEN;
			break;
		case ResourceType::MATERIAL:
			icon = ICON_FA_ADJUST;
			break;
		case ResourceType::SOUND:
			icon = ICON_FA_VOLUME_UP;
			break;
		case ResourceType::SCENE:
			icon = ICON_FA_SIMPLYBUILT;
			break;
		case ResourceType::NAVMESH:
			icon = ICON_FA_BRAIN;
			break;
		case ResourceType::FONT:
			icon = ICON_FA_FONT;
			break;
		case ResourceType::SKYBOX:
			icon = ICON_FA_CLOUD_MOON;
			break;
		case ResourceType::SKELETON:
			icon = ICON_FA_USER;
			break;
		case ResourceType::STATE_MACHINE:
			icon = ICON_FA_PROJECT_DIAGRAM;
			break;
		default:
			icon = ICON_FA_FILE;
			break;
		}

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);

		static ImVec4 color = ImVec4(0, 0.4, 0.6, 1);
		ImGui::PushStyleColor(ImGuiCol_Button, color);

		ImGui::Button(icon.c_str(),ImVec2(0.75*file_size_width, 0.75*file_size_width));
		ImGui::PopStyleColor(1);
	}
	
	ImGui::SetWindowFontScale(1);
}

size_t PanelProjectExplorer::GetResourcePreviewImage(uint32_t uuid)
{
	size_t opengl_id = 0;
	if (project_explorer_icon_cache.find(uuid) == project_explorer_icon_cache.end())
	{
		opengl_id = (project_explorer_icon_cache[uuid] = App->resources->Load<Texture>(uuid))->opengl_texture;
	}
	else
	{
		opengl_id = project_explorer_icon_cache[uuid]->opengl_texture;
	}

	return opengl_id;
}

void PanelProjectExplorer::ApplyRename()
{
	if (renaming_file)
	{
		App->filesystem->Rename(renaming_file, new_name_file);
		renaming_file = nullptr;
	}
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
void PanelProjectExplorer::ResourceDropTarget(Path * folder_path) const
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Resource"))
		{
			assert(payload->DataSize == sizeof(Metafile*));
			Metafile* incoming_resource_metafile = *(Metafile**)payload->Data;
			App->filesystem->Move(incoming_resource_metafile->imported_file_path, folder_path->GetFullPath());
			Path* new_metafile = App->filesystem->Move(incoming_resource_metafile->metafile_path, folder_path->GetFullPath());
			App->resources->metafile_manager->RefreshMetafile(*new_metafile);
		}
		ImGui::EndDragDropTarget();
	}
}
void PanelProjectExplorer::ProcessResourceMouseInput(Path* metafile_path, Metafile* metafile)
{
	if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(0))
	{
		selected_file = metafile_path;
		App->editor->selected_meta_file = metafile;
		App->editor->show_game_object_inspector = false;
		
	}
	if (ImGui::IsWindowHovered() && ImGui::IsMouseDoubleClicked(0))
	{
		if (metafile->resource_type == ResourceType::STATE_MACHINE)
		{
			App->editor->state_machine->SwitchOpen();
			App->editor->state_machine->OpenStateMachine(metafile->uuid);
		}
	}
}

void PanelProjectExplorer::ProcessMouseInput(Path* file_path)
{
	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			//selected_folder = file_path;
			selected_file = nullptr;
			ApplyRename();
		}
		if (ImGui::IsMouseDoubleClicked(0))
		{
			selected_folder = file_path;
			project_explorer_icon_cache.clear();
		}
	}
}

void PanelProjectExplorer::ShowFileSystemActionsMenu(Path* path)
{
	if (path != nullptr && hovered && ImGui::IsMouseClicked(1))
	{
		ImGui::OpenPopup("Menu");
	}
	if (ImGui::BeginPopup("Menu"))
	{
		if (selected_folder != nullptr && path->IsDirectory() && ImGui::BeginMenu("Create"))
		{
			if (ImGui::Selectable("Folder"))
			{
				std::string new_directory_path = path->GetFullPath() + "/New folder";
				size_t i = 0;
				while (App->filesystem->Exists(new_directory_path))
				{
					new_directory_path = path->GetFullPath() + "/New folder" +" " + std::to_string(i);
					i++;
				}
				App->filesystem->MakeDirectory(new_directory_path);
			}
			ImGui::Separator();
			if (ImGui::Selectable("Material"))
			{
				App->resources->Create<Material>(*selected_folder, "New Material.mat");

			}
			if (ImGui::Selectable("Skybox"))
			{
				App->resources->Create<Skybox>(*selected_folder, "New Skybox.skybox");
			}
			if (ImGui::Selectable("State Machine"))
			{
				App->resources->Create<StateMachine>(*selected_folder, "New State Machine.stm");
			}
			ImGui::EndMenu();
		}
		if (ImGui::Selectable("Reimport"))
		{
			App->resources->ImportAssetsInDirectory(*selected_folder, true);
			App->resources->CleanResourceCache();
		}
		if (ImGui::Selectable("Reimport All"))
		{
			App->resources->ImportAssetsInDirectory(*App->filesystem->GetRootPath(), true);
			App->resources->CleanResourceCache();
		}
		if (selected_file != nullptr)
		{
			if (ImGui::Selectable("Delete"))
			{
				bool success = App->filesystem->Remove(selected_file);
				if (success)
				{
					selected_file = nullptr;
					renaming_file = nullptr;
				}
			}
			if (App->editor->selected_meta_file && App->editor->selected_meta_file->resource_type == ResourceType::MODEL && ImGui::Selectable("Extract Prefab"))
			{
				std::string original_model_name = selected_file->GetFilenameWithoutExtension();
				std::string new_prefab_name = original_model_name.substr(0, original_model_name.find_last_of(".")) + ".prefab";
				Path * new_prefab = App->filesystem->Copy(App->editor->selected_meta_file->exported_file_path.c_str(), selected_folder->GetFullPath(),new_prefab_name.c_str());
				App->resources->Import(*new_prefab);
			}
			if (ImGui::Selectable("Rename"))
			{
				renaming_file = selected_file;
				new_name_file = selected_file->GetFilename();
			}
		}
		ImGui::EndPopup();
	}
	bool enter_pressed = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter));
	if (enter_pressed || renaming_file && renaming_file != selected_file)
	{
		ApplyRename();
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
			uint32_t prefab_uuid = PrefabManager::CreateFromGameObject(*selected_folder, *incoming_game_object);
			if (prefab_uuid != 0)
			{
				App->scene->RemoveGameObject(incoming_game_object);
				std::shared_ptr<Prefab> prefab = App->resources->Load<Prefab>(prefab_uuid);
				App->editor->selected_game_object = prefab->Instantiate(App->scene->GetRoot());
			}
		}
		ImGui::EndDragDropTarget();
	}
}