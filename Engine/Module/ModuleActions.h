#ifndef _MODULEACTIONS_H_
#define _MODULEACTIONS_H_

#include "Module.h"
#include "Actions/EditorAction.h"
#include "ResourceManagement/Resources/Texture.h"

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
		ADD_GAMEOBJECT,
		DELETE_GAMEOBJECT,
		ADD_COMPONENT,
		DELETE_COMPONENT,
		ENABLE_DISABLE_COMPONENT,
		EDIT_COMPONENTLIGHT,
		EDIT_COMPONENTMATERIAL,
		EDIT_COMPONENTCAMERA,
	};

	ModuleActions() = default;
	~ModuleActions() = default;

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	bool CleanUp() override;

	void ClearRedoStack();
	void ClearUndoStack();
	void Undo();
	void Redo();

	void AddUndoAction(UndoActionType type);
	void DeleteComponentUndo(Component* component);
	void ClearUndoRedoStacks();

private:
	void HandleInput();

public:
	// UndoRedo
	bool control_key_down = false;
	bool clicked = false;
	std::vector<EditorAction*> undoStack;
	std::vector<EditorAction*> redoStack;
	float3 previous_transform = float3::zero;
	GameObject* action_game_object = nullptr;
	Component* action_component = nullptr;

	float previous_light_color[3];
	float previous_light_intensity;

	Texture::TextureType type_texture;
	const int MAXIMUM_SIZE_STACK_UNDO = 5;

};

#endif _MODULEACTIONS_H_