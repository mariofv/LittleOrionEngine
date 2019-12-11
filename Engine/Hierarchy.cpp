#include "Hierarchy.h"
#include "Application.h"
#include "Component/ComponentCamera.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleScene.h"
#include "GameObject.h"

#include "imgui.h"
#include <FontAwesome5/IconsFontAwesome5.h>

Hierarchy::Hierarchy()
{
}


Hierarchy::~Hierarchy()
{
}


std::string Hierarchy::GetNextGameObjectName()
{
	char tmp_string[64];
	sprintf_s(tmp_string, "GameObject (%d)", num_game_objects++);

	return std::string(tmp_string);
}

int Hierarchy::GetNextBranch()
{
	return ++branch_counter;
}

void Hierarchy::ShowHierarchyWindow()
{
	if (ImGui::Begin(ICON_FA_SITEMAP " Hierarchy"))
	{
		for (unsigned int i = 0; i < App->scene->GetRoot()->children.size(); ++i)
		{
			ImGui::PushID(i);
			ShowGameObjectHierarchy(App->scene->GetRoot()->children[i]);
			ImGui::PopID();
		}

		ImGui::BeginChild("Hierarchy###"); // This children window is used to create a clickable area for creating and dropping game objects in the root node
		ImGui::EndChild();
		DropTarget(App->scene->GetRoot());
		if (ImGui::BeginPopupContextItem("Hierarchy###"))
		{
			if (ImGui::Selectable("Create GameObject"))
			{
				App->scene->CreateGameObject();
			}
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void Hierarchy::ShowGameObjectHierarchy(GameObject *game_object)
{
	std::string game_object_name_label = (std::string(ICON_FA_CUBE) + " " + game_object->name);
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen;
	if (game_object->children.size() == 0)
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}
	if (selected_game_object == game_object)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	bool expanded = ImGui::TreeNodeEx(game_object_name_label.c_str(), flags);
	DragAndDrop(game_object);
	ShowGameObjectActionsMenu(game_object);
	ProcessMouseInput(game_object);

	if (expanded)
	{
		for (int i = 0; i < game_object->children.size(); i++)
		{
			ImGui::PushID(i);
			ShowGameObjectHierarchy(game_object->children[i]);
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
}

void Hierarchy::DragAndDrop(GameObject *game_object)
{
	DragSource(game_object);
	DropTarget(game_object);
}


void Hierarchy::DragSource(GameObject *source_game_object)
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload("DND_GameObject", &source_game_object, sizeof(GameObject*));
		ImGui::Text("Dragging %s", source_game_object->name.c_str());
		ImGui::EndDragDropSource();
	}
}

void Hierarchy::DropTarget(GameObject *target_game_object)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_GameObject"))
		{
			assert(payload->DataSize == sizeof(GameObject*));
			GameObject *incoming_game_object = *(GameObject**)payload->Data;
			if (!incoming_game_object->IsAboveInHierarchy(*target_game_object))
			{
				incoming_game_object->SetParent(target_game_object);
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void Hierarchy::ShowGameObjectActionsMenu(GameObject *game_object)
{
	std::string label = (std::string(ICON_FA_CUBE) + " " + game_object->name);

	if (ImGui::BeginPopupContextItem(label.c_str()))
	{
		ImGui::Text(game_object->name.c_str());
		ImGui::Separator();

		if (ImGui::Selectable("Create GameObject"))
		{
			App->scene->CreateChildGameObject(game_object);
		}
		if (ImGui::Selectable("Delete GameObject"))
		{
			App->scene->RemoveGameObject(game_object);
			selected_game_object = nullptr;
		}
		if (ImGui::Selectable("Move Up"))
		{
			game_object->MoveUpInHierarchy();
		}
		if (ImGui::Selectable("Move Down"))
		{
			game_object->MoveDownInHierarchy();
		}
		ImGui::EndPopup();
	}
}

void Hierarchy::ProcessMouseInput(GameObject *game_object)
{
	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			selected_game_object = game_object;
		}

		if (ImGui::IsMouseDoubleClicked(0))
		{
			App->cameras->scene_camera->Center(game_object->aabb.bounding_box);
		}
	}
}