#include "ModuleActions.h"

#include "Actions/EditorActionEnableDisableComponent.h"
#include "Actions/EditorActionModifyCamera.h"
#include "Actions/EditorActionModifyLight.h"
#include "Actions/EditorActionAddComponent.h"
#include "Actions/EditorActionDeleteComponent.h"
#include "Actions/EditorActionAddGameObject.h"
#include "Actions/EditorActionAddMultipleGO.h"
#include "Actions/EditorActionDeleteMultipleGO.h"
#include "Actions/EditorActionDeleteGameObject.h"
#include "Actions/EditorActionMultipleRotation.h"
#include "Actions/EditorActionMultipleScale.h"
#include "Actions/EditorActionMultipleTranslate.h"
#include "Actions/EditorActionTranslate.h"
#include "Actions/EditorActionRotation.h"
#include "Actions/EditorActionScale.h"
#include "Actions/EditorAction.h"

#include "EditorUI/Panel/PanelPopups.h"
#include "Filesystem/PathAtlas.h"
#include "Log/EngineLog.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "Main/Application.h"
#include "Main/GameObject.h"

#include <Brofiler/Brofiler.h>
bool ModuleActions::Init()
{
	//Delete all actions (go are deleted here)
	ClearUndoRedoStacks();
	return true;
}


update_status ModuleActions::Update()
{
	BROFILER_CATEGORY("Module Actions Update", Profiler::Color::BlanchedAlmond);
#if GAME
	return update_status::UPDATE_CONTINUE;
#endif 
	HandleInput();
	return update_status::UPDATE_CONTINUE;
}

bool ModuleActions::CleanUp()
{
	copied_component = nullptr;
	return true;
}


void ModuleActions::ClearRedoStack()
{
	//TODO: delete all actions when engine closes (delete/add go/comp could end up in memory leak, be careful)
	while (!redoStack.empty())
	{
		EditorAction* action = redoStack.back();
		delete action;
		redoStack.pop_back();
	}
}

void ModuleActions::ClearUndoStack()
{
	while (!undoStack.empty())
	{
		EditorAction* action = undoStack.back();
		delete action;
		undoStack.pop_back();
	}
}

void ModuleActions::Undo()
{
	if (!undoStack.empty())
	{
		EditorAction* action = undoStack.back();
		action->Undo();

		redoStack.push_back(action);
		undoStack.pop_back();
	}
}

void ModuleActions::Redo()
{
	if (!redoStack.empty())
	{
		EditorAction* action = redoStack.back();
		action->Redo();

		undoStack.push_back(action);
		redoStack.pop_back();
	}
}

void ModuleActions::AddUndoAction(UndoActionType type)
{
	//StackUndoRedo set size maximum
	if (undoStack.size() >= MAXIMUM_SIZE_STACK_UNDO)
	{
		EditorAction* action = undoStack.front();
		delete action;
		undoStack.erase(undoStack.begin());
	}

	EditorAction* new_action = nullptr;

	switch (type)
	{
	case UndoActionType::TRANSLATION:
		new_action = new EditorActionTranslate(
			previous_transform,
			App->editor->selected_game_object->transform.GetTranslation(),
			App->editor->selected_game_object
		);
		break;

	case UndoActionType::ROTATION:
		new_action = new EditorActionRotation(
			previous_transform,
			App->editor->selected_game_object->transform.GetRotationRadiants(),
			App->editor->selected_game_object
		);
		break;

	case UndoActionType::SCALE:
		new_action = new EditorActionScale(
			previous_transform,
			App->editor->selected_game_object->transform.GetScale(),
			App->editor->selected_game_object
		);
		break;

	case UndoActionType::MULTIPLE_TRANSLATION:
		new_action = new EditorActionMultipleTranslate(
			previous_transform, 
			App->editor->selected_game_object->transform.GetTranslation()
		);
		break;

	case UndoActionType::MULTIPLE_SCALE:
		new_action = new EditorActionMultipleScale(
			previous_transform,
			App->editor->selected_game_object->transform.GetScale()
		);
		break;

	case UndoActionType::MULTIPLE_ROTATION:
		new_action = new EditorActionMultipleRotate(
			previous_transform,
			App->editor->selected_game_object->transform.GetRotationRadiants()
		);
		break;

	case UndoActionType::ADD_GAMEOBJECT:
		new_action = new EditorActionAddGameObject(
			action_game_object
		);
		break;

	case UndoActionType::ADD_MULTIPLE_GO:
		new_action = new EditorActionAddMultipleGO();
		break;

	case UndoActionType::DELETE_MULTIPLE_GO:
		new_action = new EditorActionDeleteMultipleGO();
		break;

	case UndoActionType::DELETE_GAMEOBJECT:
		new_action = new EditorActionDeleteGameObject(
			action_game_object
		);
		break;

	case UndoActionType::ADD_COMPONENT:
		new_action = new EditorActionAddComponent(action_component);
		break;

	case UndoActionType::DELETE_COMPONENT:
		new_action = new EditorActionDeleteComponent(action_component);
		break;

	case UndoActionType::EDIT_COMPONENTLIGHT:
		new_action = new EditorActionModifyLight(
			(ComponentLight*)action_component,
			previous_light_color,
			previous_light_intensity
		);
		break;
/*
	case UndoActionType::EDIT_COMPONENTMATERIAL:
		new_action = new EditorActionSetTexture(
			(ComponentMaterialRenderer*)action_component,
			type_texture
		);
		break;
*/
	case UndoActionType::EDIT_COMPONENTCAMERA:
		new_action = new EditorActionModifyCamera(
			(ComponentCamera*)action_component
		);
		break;

	case UndoActionType::ENABLE_DISABLE_COMPONENT:
		new_action = new EditorActionEnableDisableComponent(action_component);
		break;

	default:
		break;
	}
	if (new_action != nullptr)
	{
		undoStack.push_back(new_action);
		ClearRedoStack();
	}
}

void ModuleActions::DeleteComponentUndo(Component * component)
{
	//UndoRedo
	action_component = component;
	AddUndoAction(UndoActionType::DELETE_COMPONENT);
	component->owner->RemoveComponent(component);
}

void ModuleActions::PasteComponent(GameObject* gameobject)
{
	assert(gameobject);
	if (copied_component != nullptr)
	{
		copied_component->Clone(gameobject, gameobject->original_prefab);
	}
	
}

void ModuleActions::PasteComponentValues(Component * component)
{
	if (copied_component != nullptr && component->type == copied_component->type) 
	{
		copied_component->CopyTo(component);
	}
}

void ModuleActions::SetCopyComponent(Component * component)
{
	copied_component = component;
}


void ModuleActions::ClearUndoRedoStacks()
{
	ClearRedoStack();
	ClearUndoStack();
}

void ModuleActions::CheckClickForUndo(UndoActionType type, Component * component)
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
		case ModuleActions::UndoActionType::EDIT_COMPONENTLIGHT:
			App->actions->previous_light_color[0] = ((ComponentLight*)component)->light_color[0];
			App->actions->previous_light_color[1] = ((ComponentLight*)component)->light_color[1];
			App->actions->previous_light_color[2] = ((ComponentLight*)component)->light_color[2];
			App->actions->previous_light_intensity = ((ComponentLight*)component)->light_intensity;
			App->actions->action_component = component;
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

void ModuleActions::HandleInput()
{
	if (active_macros)
	{
		DuplicateMacros();
		DeleteMacros();
		GuizmoMacros();
	}
	
	UndoRedoMacros();
	SceneMacros();
}

void ModuleActions::UndoRedoMacros()
{
	if (App->input->GetKeyDown(KeyCode::Z))
	{
		control_key_down = true;
	}
	if (App->input->GetKeyUp(KeyCode::LeftControl))
	{
		control_key_down = false;
	}
	if (control_key_down && App->input->GetKey(KeyCode::LeftControl) && !App->input->GetKey(KeyCode::LeftShift))
	{
		Undo();
		control_key_down = false;
	}
	if (control_key_down && App->input->GetKey(KeyCode::LeftControl) && App->input->GetKey(KeyCode::LeftShift))
	{
		Redo();
		control_key_down = false;
	}
}

void ModuleActions::DuplicateMacros()
{
	if (App->input->GetKey(KeyCode::LeftControl) && App->input->GetKeyDown(KeyCode::D))
	{	
		App->actions->AddUndoAction(ModuleActions::UndoActionType::ADD_MULTIPLE_GO);
		App->scene->DuplicateGameObjectList(App->editor->selected_game_objects);	
	}
}

void ModuleActions::DeleteMacros()
{
	if (App->input->GetKeyDown(KeyCode::Delete))
	{
		App->actions->AddUndoAction(ModuleActions::UndoActionType::DELETE_MULTIPLE_GO);
		for (auto go : App->editor->selected_game_objects)
		{
			App->scene->RemoveGameObject(go);
		}
		App->editor->selected_game_object = nullptr;
		App->editor->selected_game_objects.erase(App->editor->selected_game_objects.begin(), App->editor->selected_game_objects.end());
	}
}

void ModuleActions::SceneMacros()
{
	if (App->input->GetKey(KeyCode::LeftControl) && App->input->GetKeyDown(KeyCode::S) && !App->input->GetKey(KeyCode::LeftShift))
	{
		//Differenciate when we have to save as or save normally
		if(App->scene->CurrentSceneIsSaved())
		{
			//Save Scene normally
			App->scene->SaveScene();
		}
		else
		{
			if (!App->time->isGameRunning())
			{
				App->editor->popups->scene_saver_popup.popup_shown = true;
			}
			else
			{
				APP_LOG_INFO("You must stop play mode to save scene.");
			}
		}
	}

	if (App->input->GetKey(KeyCode::LeftControl) && App->input->GetKeyDown(KeyCode::S) && App->input->GetKey(KeyCode::LeftShift))
	{
		if (!App->time->isGameRunning())
		{
			App->editor->popups->scene_saver_popup.popup_shown = true;
		}
		else
		{
			APP_LOG_INFO("You must stop play mode to save scene.");
		}

	}

	if (App->input->GetKey(KeyCode::LeftControl) && App->input->GetKeyDown(KeyCode::N))
	{
		App->scene->OpenNewScene();
	}

	if (App->input->GetKey(KeyCode::LeftControl) && App->input->GetKeyDown(KeyCode::O))
	{
		App->editor->popups->scene_loader_popup.popup_shown = true;
	}
}

void ModuleActions::GuizmoMacros()
{
	if (App->input->GetKeyDown(KeyCode::W) && !App->input->GetMouseButton(MouseButton::Right) && !App->time->isGameRunning())
	{
		App->editor->gizmo_operation = ImGuizmo::TRANSLATE;
	}
	if (App->input->GetKeyDown(KeyCode::E) && !App->input->GetMouseButton(MouseButton::Right) && !App->time->isGameRunning())
	{
		App->editor->gizmo_operation = ImGuizmo::ROTATE;
	}
	if (App->input->GetKeyDown(KeyCode::R) && !App->input->GetMouseButton(MouseButton::Right) && !App->time->isGameRunning())
	{
		App->editor->gizmo_operation = ImGuizmo::SCALE;
	}
}

