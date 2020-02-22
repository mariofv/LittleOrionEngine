#ifndef _MODULEEDITOR_H_
#define _MODULEEDITOR_H_

#include "Module.h"
#include "Actions/EditorAction.h"
#include "Main/Globals.h"
#include "ResourceManagement/Resources/Texture.h"

#include <vector>
#include <ImGuizmo.h>
#include <MathGeoLib.h>

class Component;

class Panel;
class PanelMenuBar;
class PanelToolBar;
class PanelScene;
class PanelGame;
class PanelInspector;
class PanelHierarchy;
class PanelProjectExplorer;
class PanelConsole;
class PanelDebug;
class PanelConfiguration;
class PanelAbout;
class PanelPopups;

class GameObject;

struct ImFont;

class ModuleEditor : public Module
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

	ModuleEditor() = default;
	~ModuleEditor() = default;

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	bool CleanUp() override;

	void OpenScene(const std::string &path) const;
	void SaveScene(const std::string &path) const;

	void Render();

	ImFont* GetFont(const Fonts & font) const;

	void ClearRedoStack();
	void ClearUndoStack();
	void Undo();
	void Redo();

	void AddUndoAction(UndoActionType type);
	void DeleteComponentUndo(Component* component);
	void ClearUndoRedoStacks();

private:
	void InitEditorDockspace();
	void RenderEditorDockspace();

	bool InitImgui();
	void LoadFonts();

	void HandleInput();

public:
	GameObject *selected_game_object = nullptr;
	ImGuizmo::OPERATION gizmo_operation = ImGuizmo::TRANSLATE;

	// UndoRedo

	bool control_key_down = false;
	bool clicked = false;
	bool clicked_light = false;
	bool clicked_camera = false;
	std::vector<EditorAction*> undoStack;
	std::vector<EditorAction*> redoStack;
	float3 previous_transform = float3::zero;
	GameObject* action_game_object = nullptr;
	Component* action_component = nullptr;

	float previous_light_color[3];
	float previous_light_intensity;
	
	Texture::TextureType type_texture;
	const int MAXIMUM_SIZE_STACK_UNDO = 5;

	// UndoRedo End

	PanelMenuBar* menu_bar = nullptr;
	PanelToolBar* toolbar = nullptr;
	PanelScene* scene_panel = nullptr;
	PanelGame* game_panel = nullptr;
	PanelInspector* inspector = nullptr;
	PanelHierarchy* hierarchy = nullptr;
	PanelProjectExplorer* project_explorer = nullptr;
	PanelConsole* console = nullptr;
	PanelDebug* debug_panel = nullptr;
	PanelConfiguration* configuration = nullptr;
	PanelAbout* about = nullptr;
	PanelPopups* popups = nullptr;

private:
	std::vector<Panel*> panels;
	ImGuiID editor_dockspace_id;
};

#endif //_MODULEEDITOR_H_