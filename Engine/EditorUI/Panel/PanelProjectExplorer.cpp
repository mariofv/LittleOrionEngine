#include "PanelProjectExplorer.h"

#include "EditorUI/Panel/PanelInspector.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelMetaFile.h"
#include "EditorUI/Panel/PanelPopups.h"

#include "Filesystem/PathAtlas.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTexture.h"

#include "ResourceManagement/Manager/SkyboxManager.h"
#include "ResourceManagement/Metafile/Metafile.h"
#include "ResourceManagement/Metafile/ModelMetafile.h"
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


PanelProjectExplorer::PanelProjectExplorer()
{
	opened = true;
	enabled = true;
	window_name = ICON_FA_FOLDER " Project Explorer";
}

void PanelProjectExplorer::Render()
{
	BROFILER_CATEGORY("Render Project Explorer", Profiler::Color::BlueViolet);

	if (ImGui::Begin(ICON_FA_FOLDER " Project Explorer", &opened))
	{
		hovered = ImGui::IsWindowHovered();

		ImGui::SameLine();
		ImGui::Text(ICON_FA_SEARCH);

		ImGui::SameLine();
		if(ImGui::InputText("###File Searching Input", &searching_file))
		{
			searching_file_paths.clear();
			SearchFilesInExplorer(App->filesystem->assets_folder_path);
		}

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
			if (searching_file == "")
			{
				ShowFilesInExplorer();
			}
			else
			{
				ShowSearchedFiles();
			}
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
	if (!App->resources->first_import_completed)
	{
		return;
	}
	for (auto & path_child : path.children)
	{
		if (path_child->IsDirectory())
		{

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

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
			else
			{
				if(IsOneOfMyChildrens(path_child))
				{
					flags |= ImGuiTreeNodeFlags_DefaultOpen;
					filename = ICON_FA_FOLDER_OPEN " " + path_child->GetFilename();
				}
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

void PanelProjectExplorer::ShowFoldersHierarchySearch(const Path & path)
{
	
	transform(searching_file.begin(), searching_file.end(), searching_file.begin(), ::tolower);
	for (auto & path_child : path.children)
	{
		std::string name = path_child->GetFilename();
		transform(name.begin(), name.end(), name.begin(), ::tolower);
		if (path_child->IsDirectory() && name.find(searching_file) != std::string::npos)
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
		
		if (path.IsDirectory() && path_child->sub_folders > 0)
		{
			ShowFoldersHierarchySearch(*path_child);				
		}
		
	}
}

void PanelProjectExplorer::ShowFilesInExplorer()
{
	if (selected_folder == nullptr)
	{
		selected_folder = App->filesystem->assets_folder_path;
		return;
	}

	ImVec2 available_region = ImGui::GetContentRegionAvail();
	int files_per_line = static_cast<int>(available_region.x / file_size_width);

	int current_line = 0;
	int current_file_in_line = 0;

	for (auto & child_path : selected_folder->children)
	{
		if (child_path != nullptr && child_path->IsMeta())
		{
			ImGui::PushID(current_line * files_per_line + current_file_in_line);
			Metafile* metafile = App->resources->metafile_manager->GetMetafile(*child_path);
			std::string filename = child_path->GetFilenameWithoutExtension();
			ShowMetafile(child_path,metafile,filename);
			ImGui::PopID();
			CalculateNextLinePosition(current_file_in_line, files_per_line, current_line);
			if (opened_model && metafile->uuid == opened_model->uuid)
			{
				for (auto & meta : opened_model->nodes)
				{
					ImGui::PushID(meta->resource_name.c_str());
					ShowMetafile(App->filesystem->GetPath(meta->exported_file_path), meta.get(), meta->resource_name);
					ImGui::PopID();
					CalculateNextLinePosition(current_file_in_line, files_per_line, current_line);
				}
			}
		}
	}
}

void PanelProjectExplorer::SearchFilesInExplorer(Path* path)
{
	BROFILER_CATEGORY("SearchFilesInExplorer", Profiler::Color::BlueViolet);

	transform(searching_file.begin(), searching_file.end(), searching_file.begin(), ::tolower);
	for (auto & child_path : path->children)
	{
		std::string name = child_path->GetFilename();
		transform(name.begin(), name.end(), name.begin(), ::tolower);
		if (child_path != nullptr && child_path->IsMeta() && name.find(searching_file) != std::string::npos)
		{
			searching_file_paths.emplace_back(child_path);
		}
		if (child_path->IsDirectory())
		{
			SearchFilesInExplorer(child_path);
		}
	}
}

void PanelProjectExplorer::CalculateNextLinePosition(int &current_file_in_line, int files_per_line, int &current_line)
{

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

void PanelProjectExplorer::ShowMetafile(Path* metafile_path, Metafile* metafile,const std::string& filename)
{
	BROFILER_CATEGORY("ShowMetafile", Profiler::Color::BlueViolet);

	bool is_model = metafile->resource_type == ResourceType::MODEL;

	float size_plus = is_model ? 30.f : 0.f;
	if (ImGui::BeginChild(filename.c_str(), ImVec2(file_size_width + size_plus, file_size_height), selected_file == metafile_path, ImGuiWindowFlags_NoDecoration))
	{
		hovered = ImGui::IsWindowHovered() ? true : hovered;

		ResourceDragSource(metafile);
		ProcessResourceMouseInput(metafile_path, metafile);

		ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 0.75f * file_size_width) * 0.5f);
		ShowMetafileIcon(metafile);
		if (is_model)
		{
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_PLAY))
			{
				opened_model = opened_model && opened_model->uuid == metafile->uuid ? nullptr :static_cast<ModelMetafile*>(metafile);
			}
		}
		ImGui::Spacing();

		float text_width = ImGui::CalcTextSize(filename.c_str()).x;
		if (text_width < file_size_width)
		{
			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_width) * 0.5f);
			ImGui::Text(filename.c_str());
		}
		else
		{
			float character_width = text_width / filename.length();
			size_t string_position_wrap = static_cast<size_t>(file_size_width / character_width - 5);
			assert(string_position_wrap < filename.length());
			std::string wrapped_filename = filename.substr(0, string_position_wrap + 3) + "\n" + filename.substr(string_position_wrap, filename.size());
			ImGui::Text(wrapped_filename.c_str());
		}
		
	}
	ImGui::EndChild();

}

void PanelProjectExplorer::ShowMetafileIcon(Metafile * metafile)
{
	BROFILER_CATEGORY("Show metafile icon", Profiler::Color::BlueViolet);

	ImGui::SetWindowFontScale(2);

	if(metafile->resource_type == ResourceType::TEXTURE)
	{
		ImGui::Image((void *)GetResourcePreviewImage(metafile->uuid), ImVec2(0.75f*file_size_width, 0.75f*file_size_width));
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
		case ResourceType::VIDEO:
			icon = ICON_FA_FILM;
			break;
		default:
			icon = ICON_FA_FILE;
			break;
		}

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);

		static ImVec4 color = ImVec4(0.f, 0.4f, 0.6f, 1.f);
		ImGui::PushStyleColor(ImGuiCol_Button, color);

		ImGui::Button(icon.c_str(),ImVec2(0.75f*file_size_width, 0.75f*file_size_width));
		ImGui::PopStyleColor(1);
		ImGui::PopItemFlag();
	}
	
	ImGui::SetWindowFontScale(1);
}

size_t PanelProjectExplorer::GetResourcePreviewImage(uint32_t uuid)
{
	size_t opengl_id = 0;
	if (project_explorer_icon_cache.find(uuid) == project_explorer_icon_cache.end())
	{
		App->resources->loading_thread_communication.normal_loading_flag = true;
		opengl_id = (project_explorer_icon_cache[uuid] = App->resources->Load<Texture>(uuid))->opengl_texture;
		App->resources->loading_thread_communication.normal_loading_flag = false;
	}
	else
	{
		opengl_id = project_explorer_icon_cache[uuid]->opengl_texture;
	}

	return opengl_id;
}

void PanelProjectExplorer::ShowSearchedFiles()
{
	BROFILER_CATEGORY("ShowSearchedFiles", Profiler::Color::BlueViolet);

	ImVec2 available_region = ImGui::GetContentRegionAvail();
	int files_per_line = static_cast<int>(available_region.x / file_size_width);

	int current_line = 0;
	int current_file_in_line = 0;

	for(auto& path : searching_file_paths)
	{
		ImGui::PushID(current_line * files_per_line + current_file_in_line);
		Metafile* metafile = App->resources->metafile_manager->GetMetafile(*path);
		std::string filename = path->GetFilenameWithoutExtension();
		ShowMetafile(path, metafile, filename);
		ImGui::PopID();
		CalculateNextLinePosition(current_file_in_line, files_per_line, current_line);
		if (opened_model && metafile->uuid == opened_model->uuid)
		{
			for (auto & meta : opened_model->nodes)
			{
				ImGui::PushID(meta->resource_name.c_str());
				ShowMetafile(App->filesystem->GetPath(meta->exported_file_path), meta.get(), meta->resource_name);
				ImGui::PopID();
				CalculateNextLinePosition(current_file_in_line, files_per_line, current_line);
			}
		}
	}
}

void PanelProjectExplorer::ResourceDragSource(const Metafile* metafile) const
{
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("DND_Resource", &metafile, sizeof(Metafile*));
		ImGui::Text("Dragging %s", metafile->resource_name.c_str());
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

void PanelProjectExplorer::ResourceDropFromOutside(const std::string& dropped_filedir)
{
	if (!selected_folder)
	{
		return;
	}
	FileData data = App->filesystem->LoadFromSystem(dropped_filedir);
	if (data.buffer)
	{
		std::string full_path = selected_folder->GetFullPath() + "/"+dropped_filedir.substr(dropped_filedir.find_last_of("\\") +1 );
		Path* new_file = App->filesystem->Save(full_path, data);

		if (new_file && new_file->IsImportable())
		{
			App->resources->Import(*new_file);
		}
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
			App->editor->state_machine->Open();
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
			selected_file = nullptr;
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
		if (selected_folder != nullptr && path->IsDirectory())
		{
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::Selectable("Folder"))
				{
					auto & choose_name_popup = App->editor->popups->new_filename_chooser;
					choose_name_popup.show_new_filename_popup = true;
					choose_name_popup.new_filename = "New folder";
					choose_name_popup.apply_new_name = [path] (std::string & new_directory_path){
		
						new_directory_path = path->GetFullPath() + "/" + new_directory_path;
						size_t i = 0;
						while (App->filesystem->Exists(new_directory_path))
						{
							new_directory_path = path->GetFullPath() + "/New folder" + " " + std::to_string(i);
							i++;
						}
						App->filesystem->MakeDirectory(new_directory_path);
					};
				}
				ImGui::Separator();
				if (ImGui::Selectable("Material"))
				{
					auto & choose_name_popup = App->editor->popups->new_filename_chooser;
					choose_name_popup.show_new_filename_popup = true;
					choose_name_popup.new_filename = "New Material";
					choose_name_popup.apply_new_name = [this](std::string & new_directory_path) {

						App->resources->Create<Material>(*selected_folder, new_directory_path +".mat");
					};


				}
				if (ImGui::Selectable("Skybox"))
				{
					auto & choose_name_popup = App->editor->popups->new_filename_chooser;
					choose_name_popup.show_new_filename_popup = true;
					choose_name_popup.new_filename = "New Skybox";
					choose_name_popup.apply_new_name = [this](std::string & new_directory_path) {

						App->resources->Create<Skybox>(*selected_folder, new_directory_path + ".skybox");
					};
				}
				if (ImGui::Selectable("State Machine"))
				{
					auto & choose_name_popup = App->editor->popups->new_filename_chooser;
					choose_name_popup.show_new_filename_popup = true;
					choose_name_popup.new_filename = "New State Machine";
					choose_name_popup.apply_new_name = [this](std::string & new_directory_path) {

						App->resources->Create<StateMachine>(*selected_folder, new_directory_path + ".stm");
					};
				}
				ImGui::EndMenu();
			}
		}

		if (selected_file != nullptr )
		{
			std::string filename_no_extension = selected_file->GetFilenameWithoutExtension();
			bool has_uuid = std::all_of(filename_no_extension.begin(), filename_no_extension.end(), ::isdigit);
			if (!has_uuid && ImGui::Selectable("Delete"))
			{
				bool success = App->filesystem->Remove(selected_file);
				if (success)
				{
					selected_file = nullptr;
					opened_model = nullptr;
				}
			}
			if (!has_uuid && ImGui::Selectable("Rename"))
			{
				auto & choose_name_popup = App->editor->popups->new_filename_chooser;
				choose_name_popup.show_new_filename_popup = true;
				std::string original_file = selected_file->GetFilenameWithoutExtension();
				std::string extension = original_file.substr(original_file.find_last_of("."));
				choose_name_popup.new_filename = original_file.substr(0, original_file.find_last_of("."));
				choose_name_popup.apply_new_name = [this, extension](std::string & new_filename) {

					new_filename +=  extension + ".meta";
					App->filesystem->Rename(selected_file, new_filename);
				};
			}
			ImGui::Separator();
			Metafile * selected_metafile = App->editor->selected_meta_file;
			if (selected_metafile && selected_metafile->resource_type == ResourceType::MODEL && ImGui::Selectable("Extract Prefab"))
			{
				std::string new_prefab_name = filename_no_extension.substr(0, filename_no_extension.find_last_of(".")) + ".prefab";
				Path * new_prefab = App->filesystem->Copy(selected_metafile->exported_file_path.c_str(), selected_folder->GetFullPath(),new_prefab_name.c_str());
				App->resources->Import(*new_prefab);
				ImGui::Separator();
			}

			if (has_uuid && selected_metafile->resource_type == ResourceType::MATERIAL && ImGui::Selectable("Extract from prefab"))
			{
				auto& remapped_materials = opened_model->remapped_materials;
				std::string material_key = selected_metafile->resource_name.substr(0,selected_metafile->resource_name.find_last_of('.'));
				assert(opened_model);
				assert(remapped_materials.find(material_key) != remapped_materials.end());

				Path* new_extracted_material = App->filesystem->Copy(selected_metafile->exported_file_path, selected_folder->GetFullPath(), selected_metafile->resource_name);
				assert(new_extracted_material);
				remapped_materials[material_key]  = App->resources->Import(*new_extracted_material);

				App->editor->inspector->metafile_panel.ApplyMetafileChanges(opened_model);
				ImGui::Separator();
			}
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
			CreatePrefabInSelectedFolder(incoming_game_object);
		}
		ImGui::EndDragDropTarget();
	}
}

void PanelProjectExplorer::CreatePrefabInSelectedFolder(GameObject * incoming_game_object) const
{
	Path* destination_folder = selected_folder ? selected_folder : App->filesystem->GetRootPath();
	uint32_t prefab_uuid = PrefabManager::CreateFromGameObject(*destination_folder, *incoming_game_object);
	if (prefab_uuid != 0)
	{
		std::shared_ptr<Prefab> prefab = App->resources->Load<Prefab>(prefab_uuid);
		App->editor->selected_game_object = prefab->Instantiate(incoming_game_object->parent ? incoming_game_object->parent : App->scene->GetRoot());
		App->scene->RemoveGameObject(incoming_game_object);
	}
}

bool PanelProjectExplorer::IsOneOfMyChildrens(Path* path) const
{
	bool found = false;
	for (auto& child : path->children)
	{
		found = std::find(child->children.begin(), child->children.end(), selected_folder) != child->children.end() || child == selected_folder;
		if (!found && child->children.size() > 0)
		{
			found = IsOneOfMyChildrens(child);
		}
		if (found)
		{
			break;
		}
	}
	return found;
}