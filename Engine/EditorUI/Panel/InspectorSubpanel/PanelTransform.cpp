#include "PanelTransform.h"

#include "Component/ComponentTransform.h"
#include "Component/ComponentTransform2D.h"

#include "Helper/Utils.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include <imgui.h>
#include <MathGeoLib.h>
#include <FontAwesome5/IconsFontAwesome5.h>

void PanelTransform::ShowComponentTransformWindow(ComponentTransform *transform)
{
	if (ImGui::CollapsingHeader(ICON_FA_RULER_COMBINED " Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat3("Translation", transform->translation.ptr(), 0.01f))
		{
			transform->OnTransformChange();
			transform->modified_by_user = true;
		}
		//UndoRedo
		CheckClickForUndo(ModuleActions::UndoActionType::TRANSLATION, transform);

		if (ImGui::DragFloat3("Rotation", transform->rotation_degrees.ptr(), 0.1f, -180.f, 180.f))
		{
			transform->rotation = Utils::GenerateQuatFromDegFloat3(transform->rotation_degrees);
			transform->rotation_radians = Utils::Float3DegToRad(transform->rotation_degrees);
			transform->OnTransformChange();
			transform->modified_by_user = true;
		}
		//UndoRedo
		CheckClickForUndo(ModuleActions::UndoActionType::ROTATION, transform);

		if (ImGui::DragFloat3("Scale", transform->scale.ptr(), 0.01f))
		{
			transform->OnTransformChange();
			transform->modified_by_user = true;
		}

		if (ImGui::Button("Copy"))
		{
			App->actions->SetCopyComponent(transform);
		}
		if (ImGui::Button("Paste component values"))
		{
			App->actions->PasteComponentValues(transform);
		}
		//UndoRedo
		CheckClickForUndo(ModuleActions::UndoActionType::SCALE, transform);
	}
}

void PanelTransform::ShowComponentTransform2DWindow(ComponentTransform2D *transform_2D)
{
	if (ImGui::CollapsingHeader(ICON_FA_RULER_COMBINED " Transform 2D", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Delete"))
		{
			transform_2D->owner->SetTransform2DStatus(false);
			return;
		}
		if (ImGui::Button("Copy"))
		{
			App->actions->SetCopyComponent(transform_2D);
		}
		if (ImGui::Button("Paste component values"))
		{
			App->actions->PasteComponentValues(transform_2D);
		}

		float child_width = (ImGui::GetContentRegionAvail().x - 2 * ImGui::GetStyle().ItemSpacing.x) / 3;
		child_width = math::Max(10.f, child_width);
		float child_height = 20.f;

		// ===================================

		ImGui::BeginGroup();
		if (transform_2D->HasCoincidentHorizontalAnchors())
		{
			// POS X
			ImGui::Text("Pos X");
			if (ImGui::BeginChild("###Pos X child", ImVec2(child_width, child_height)))
			{
				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat("###Pos X drag", &transform_2D->anchored_position.x, 1.0f))
				{
					transform_2D->OnTransformChange();
					transform_2D->modified_by_user = true;
				}
				ImGui::PopItemWidth();
				CheckClickForUndo(ModuleActions::UndoActionType::EDIT_RECT2D, transform_2D);
			}
			ImGui::EndChild();
		}
		else
		{
			// LEFT
			ImGui::Text("Left");
			if (ImGui::BeginChild("###Left child", ImVec2(child_width, child_height)))
			{
				float rect_left = transform_2D->rect.left;
				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat("###Left drag", &rect_left, 1.0f))
				{
					transform_2D->SetLeft(rect_left);
					transform_2D->modified_by_user = true;
				}
				ImGui::PopItemWidth();
				CheckClickForUndo(ModuleActions::UndoActionType::EDIT_RECT2D, transform_2D);
			}
			ImGui::EndChild();
		}
		ImGui::EndGroup();
		
		// ===================================

		ImGui::SameLine();
		ImGui::BeginGroup();
		if (transform_2D->HasCoincidentVerticalAnchors())
		{
			// POS Y
			ImGui::Text("Pos Y");
			if (ImGui::BeginChild("###Pos Y child", ImVec2(child_width, child_height)))
			{
				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat("###Pos Y drag", &transform_2D->anchored_position.y, 1.0f))
				{
					transform_2D->OnTransformChange();
					transform_2D->modified_by_user = true;
				}
				ImGui::PopItemWidth();
				CheckClickForUndo(ModuleActions::UndoActionType::EDIT_RECT2D, transform_2D);
			}
			ImGui::EndChild();
		}
		else
		{
			// TOP
			ImGui::Text("Top");
			if (ImGui::BeginChild("###Top child", ImVec2(child_width, child_height)))
			{
				float rect_top = transform_2D->rect.top;
				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat("###Top drag", &rect_top, 1.0f))
				{
					transform_2D->SetTop(rect_top);
					transform_2D->modified_by_user = true;
				}
				ImGui::PopItemWidth();
				CheckClickForUndo(ModuleActions::UndoActionType::EDIT_RECT2D, transform_2D);
			}
			ImGui::EndChild();
		}
		ImGui::EndGroup();

		// ===================================

		// POS Z
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::Text("Pos Z");
		if (ImGui::BeginChild("###Pos Z child", ImVec2(child_width, child_height)))
		{
			ImGui::PushItemWidth(-1);
			if (ImGui::DragFloat("###Pos Z drag", &transform_2D->translation.z, 1.0f))
			{
				transform_2D->OnTransformChange();
				transform_2D->modified_by_user = true;
			}
			ImGui::PopItemWidth();
			CheckClickForUndo(ModuleActions::UndoActionType::EDIT_RECT2D, transform_2D);
		}
		ImGui::EndChild();
		ImGui::EndGroup();

		// ===================================

		ImGui::BeginGroup();
		if (transform_2D->HasCoincidentHorizontalAnchors())
		{
			// WIDTH
			ImGui::Text("Width");
			if (ImGui::BeginChild("###Width child", ImVec2(child_width, child_height)))
			{
				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat("###Width drag", &transform_2D->size.x, 1.0f))
				{
					transform_2D->SetWidth(transform_2D->size.x);
					transform_2D->OnTransformChange();
					transform_2D->modified_by_user = true;
				}
				ImGui::PopItemWidth();
				CheckClickForUndo(ModuleActions::UndoActionType::EDIT_RECT2D, transform_2D);
			}
			ImGui::EndChild();
		}
		else
		{
			// RIGHT
			ImGui::Text("Right");
			if (ImGui::BeginChild("###Right child", ImVec2(child_width, child_height)))
			{
				float rect_right = transform_2D->rect.right;
				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat("###Right drag", &rect_right, 1.0f))
				{
					transform_2D->SetRight(rect_right);
					transform_2D->modified_by_user = true;
				}
				ImGui::PopItemWidth();
				CheckClickForUndo(ModuleActions::UndoActionType::EDIT_RECT2D, transform_2D);
			}
			ImGui::EndChild();
		}
		ImGui::EndGroup();

		// ===================================

		ImGui::SameLine();
		ImGui::BeginGroup();
		if (transform_2D->HasCoincidentVerticalAnchors())
		{
			// HEIGHT
			ImGui::Text("Height");
			if (ImGui::BeginChild("###Height child", ImVec2(child_width, child_height)))
			{
				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat("###Height drag", &transform_2D->size.y, 1.0f))
				{
					transform_2D->SetHeight(transform_2D->size.y);
					transform_2D->OnTransformChange();
					transform_2D->modified_by_user = true;
				}
				ImGui::PopItemWidth();
				CheckClickForUndo(ModuleActions::UndoActionType::EDIT_RECT2D, transform_2D);
			}
			ImGui::EndChild();
		}
		else
		{
			// BOTTOM
			ImGui::Text("Bottom");
			if (ImGui::BeginChild("###Bottom child", ImVec2(child_width, child_height)))
			{
				float rect_bottom = transform_2D->rect.bottom;
				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat("###Bottom drag", &rect_bottom, 1.0f))
				{
					transform_2D->SetBottom(rect_bottom);
					transform_2D->modified_by_user = true;
				}
				ImGui::PopItemWidth();
				CheckClickForUndo(ModuleActions::UndoActionType::EDIT_RECT2D, transform_2D);
			}
			ImGui::EndChild();
		}
		ImGui::EndGroup();

		// ===================================


		if (ImGui::TreeNodeEx("Anchors", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float2 current_min_anchor = transform_2D->min_anchor;
			if (ImGui::DragFloat2("Min", current_min_anchor.ptr(), 0.01f, 0.0f, 1.0f))
			{
				transform_2D->SetMinAnchor(current_min_anchor);
			}
			CheckClickForUndo(ModuleActions::UndoActionType::EDIT_RECT2D, transform_2D);

			float2 current_max_anchor = transform_2D->max_anchor;
			if (ImGui::DragFloat2("Max", current_max_anchor.ptr(), 0.01f, 0.0f, 1.0f))
			{
				transform_2D->SetMaxAnchor(current_max_anchor);
			}
			CheckClickForUndo(ModuleActions::UndoActionType::EDIT_RECT2D, transform_2D);

			ImGui::TreePop();
		}

		float2 current_pivot = transform_2D->pivot;
		if (ImGui::DragFloat2("Pivot", current_pivot.ptr(), 0.01f, 0.0f, 1.0f))
		{
			transform_2D->SetPivot(current_pivot);
		}
		CheckClickForUndo(ModuleActions::UndoActionType::EDIT_RECT2D, transform_2D);
		

		if (ImGui::DragFloat3("Rotation", transform_2D->rotation_degrees.ptr(), 0.1f, -180.f, 180.f))
		{
			transform_2D->rotation = Utils::GenerateQuatFromDegFloat3(transform_2D->rotation_degrees);
			transform_2D->rotation_radians = Utils::Float3DegToRad(transform_2D->rotation_degrees);
			transform_2D->OnTransformChange();
			transform_2D->modified_by_user = true;
		}
		//UndoRedo
		CheckClickForUndo(ModuleActions::UndoActionType::ROTATION, transform_2D);

		if (ImGui::DragFloat3("Scale", transform_2D->scale.ptr(), 0.01f))
		{
			transform_2D->OnTransformChange();
			transform_2D->modified_by_user = true;
		}
		//UndoRedo
		CheckClickForUndo(ModuleActions::UndoActionType::SCALE, transform_2D);
	}
}


void PanelTransform::CheckClickForUndo(ModuleActions::UndoActionType  type, Component* component)
{
	if (ImGui::IsItemActive() && !ImGui::IsItemActiveLastFrame())
	{
		switch (type)
		{
		case ModuleActions::UndoActionType::TRANSLATION:
			App->actions->previous_transform = ((ComponentTransform*)component)->GetTranslation();
			break;
		case ModuleActions::UndoActionType::ROTATION:
			App->actions->previous_transform = ((ComponentTransform*)component)->GetRotationRadiants();
			break;
		case ModuleActions::UndoActionType::SCALE:
			App->actions->previous_transform = ((ComponentTransform*)component)->GetScale();
			break;
		case ModuleActions::UndoActionType::EDIT_RECT2D:
		case ModuleActions::UndoActionType::EDIT_RECT2D_ROTATION:
			App->actions->action_component = (ComponentTransform2D*)component;
			break;
		default:
			break;
		}

		App->actions->clicked = true;
	}

	if (ImGui::IsItemDeactivatedAfterChange())
	{
		App->actions->AddUndoAction(type);
		App->actions->clicked = false;
	}

}