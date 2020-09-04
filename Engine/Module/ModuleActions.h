#ifndef _MODULEACTIONS_H_
#define _MODULEACTIONS_H_

#include "Module.h"
#include "Actions/EditorAction.h"
#include "ResourceManagement/Resources/Material.h"

#include <MathGeoLib.h>

class GameObject;
class Component;

class ModuleActions : public Module
{
public:
	enum class UndoActionType
	{
		TRANSLATION,
		ROTATION,
		SCALE,
		MULTIPLE_TRANSLATION,
		MULTIPLE_ROTATION,
		MULTIPLE_SCALE,
		ADD_GAMEOBJECT,
		DELETE_GAMEOBJECT,
		ADD_COMPONENT,
		DELETE_COMPONENT,
		PASTE_COMPONENT,
		ENABLE_DISABLE_COMPONENT,
		EDIT_COMPONENTLIGHT,
		//EDIT_COMPONENTMATERIAL,
		EDIT_COMPONENTCAMERA,
		EDIT_RECT2D,
		EDIT_RECT2D_ROTATION,
		ADD_MULTIPLE_GO,
		DELETE_MULTIPLE_GO,
	};

	ModuleActions() = default;
	~ModuleActions() = default;

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	void ClearRedoStack();
	void ClearUndoStack();
	void Undo();
	void Redo();
	
	void AddUndoAction(UndoActionType type);
	void DeleteComponentUndo(Component* component);
	void PasteComponent(GameObject* gameobject);
	void PasteComponentValues(Component* component);
	void SetCopyComponent(Component* component);
	void ClearUndoRedoStacks();

	void CheckClickForUndo(UndoActionType type, Component* component);

private:
	void HandleInput();
	void UndoRedoMacros();
	void DuplicateMacros();
	void DeleteMacros();
	void SceneMacros();
	void GuizmoMacros();

public:
	// UndoRedo
	bool control_key_down = false;
	bool clicked = false;
	bool active_macros = true;
	std::vector<EditorAction*> undoStack;
	std::vector<EditorAction*> redoStack;
	float3 previous_transform = float3::zero;
	
	GameObject* action_game_object = nullptr;
	Component* action_component = nullptr;
	Component* copied_component = nullptr;

	float previous_light_color[3];
	float previous_light_intensity;

	Material::MaterialTextureType type_texture;
	const size_t MAXIMUM_SIZE_STACK_UNDO = 5;

};

#endif _MODULEACTIONS_H_