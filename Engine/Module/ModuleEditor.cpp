#include "ModuleEditor.h"

#include "Actions/EditorActionEnableDisableComponent.h"
#include "Actions/EditorActionModifyCamera.h"
#include "Actions/EditorActionSetTexture.h"
#include "Actions/EditorActionModifyLight.h"
#include "Actions/EditorActionAddComponent.h"
#include "Actions/EditorActionDeleteComponent.h"
#include "Actions/EditorActionAddGameObject.h"
#include "Actions/EditorActionDeleteGameObject.h"
#include "Actions/EditorActionTranslate.h"
#include "Actions/EditorActionRotation.h"
#include "Actions/EditorActionScale.h"
#include "Actions/EditorAction.h"

#include "Helper/Config.h"
#include "Main/Globals.h"
#include "Main/Application.h"
#include "ModuleResourceManager.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"

#include "UI/Panel/PanelAbout.h"
#include "UI/Panel/PanelConfiguration.h"
#include "UI/Panel/PanelConsole.h"
#include "UI/Panel/PanelDebug.h"
#include "UI/Panel/PanelGame.h"
#include "UI/Panel/PanelMenuBar.h"
#include "UI/Panel/PanelHierarchy.h"
#include "UI/Panel/PanelInspector.h"
#include "UI/Panel/PanelPopups.h"
#include "UI/Panel/PanelProjectExplorer.h"
#include "UI/Panel/PanelScene.h"
#include "UI/Panel/PanelToolBar.h"

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>
#include <Brofiler/Brofiler.h>


// Called before render is available
bool ModuleEditor::Init()
{
	APP_LOG_SECTION("************ Module Editor Init ************");
	bool ret = true;
	ret = InitImgui();

	menu_bar = new PanelMenuBar();
	toolbar = new PanelToolBar();
	panels.push_back(game_panel = new PanelGame());
	panels.push_back(scene_panel = new PanelScene());
	panels.push_back(inspector = new PanelInspector());
	panels.push_back(hierarchy = new PanelHierarchy());
	panels.push_back(project_explorer = new PanelProjectExplorer());
	panels.push_back(console = new PanelConsole());
	panels.push_back(debug_panel = new PanelDebug());
	panels.push_back(configuration = new PanelConfiguration());
	panels.push_back(about = new PanelAbout());
	panels.push_back(popups = new PanelPopups());

	//Delete all actions (go are deleted here)
	ClearUndoRedoStacks();

	return ret;
}

bool ModuleEditor::InitImgui()
{
	APP_LOG_INIT("Initializing IMGUI editor");
	SDL_GLContext gl_context = ImGui::CreateContext();

	bool err = ImGui_ImplSDL2_InitForOpenGL(App->window->window, gl_context);
	if (!err)
	{
		APP_LOG_ERROR("Error initializing IMGUI editor SDL2.")
			return false;
	}

	err = ImGui_ImplOpenGL3_Init("#version 330");
	if (!err)
	{
		APP_LOG_ERROR("Error initializing IMGUI editor OpenGL3.")
			return false;
	}

	LoadFonts();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	APP_LOG_SUCCESS("IMGUI editor initialized correctly.");
	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	return update_status::UPDATE_CONTINUE;
}


update_status ModuleEditor::Update()
{
	//ImGui::ShowStyleEditor();
	//ImGui::ShowDemoWindow();

	static bool inital_scene_loaded = false;
	if (!inital_scene_loaded && App->resources->thread_comunication.finished_loading)
	{
		OpenScene(ASSIGNMENT_SCENE_PATH);
		inital_scene_loaded = true;
	}
	return update_status::UPDATE_CONTINUE;
}

void ModuleEditor::Render()
{
	BROFILER_CATEGORY("Render UI", Profiler::Color::BlueViolet);
	
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(App->window->GetWidth(), App->window->GetHeight()));
	if (ImGui::Begin("MainWindow", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		menu_bar->Render();
		toolbar->Render();

		ImGui::Separator();

		RenderEditorDockspace();
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ModuleEditor::RenderEditorDockspace()
{
	if (ImGui::BeginChild("DockSpace", ImVec2(0, 0), false, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		editor_dockspace_id = ImGui::GetID("EditorDockspace");
		bool initialized = ImGui::DockBuilderGetNode(editor_dockspace_id) != NULL;
		
		ImGui::DockSpace(editor_dockspace_id);

		if (!initialized)
		{
			InitEditorDockspace();
		}

		for (auto& panel : panels)
		{
			if (panel->IsOpened())
			{
				panel->Render();
			}
		}
	}
	ImGui::EndChild();
}

void ModuleEditor::InitEditorDockspace()
{
	ImGui::DockBuilderRemoveNode(editor_dockspace_id); // Clear out existing layout
	ImGui::DockBuilderAddNode(editor_dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
	ImVec2 child_window_size = ImGui::GetWindowSize();
	ImGui::DockBuilderSetNodeSize(editor_dockspace_id, child_window_size);

	ImGuiID dock_main_id = editor_dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.

	ImGuiID dock_id_left;
	ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_id_left);

	ImGuiID dock_id_left_upper;
	ImGuiID dock_id_left_bottom = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.25f, NULL, &dock_id_left_upper);

	ImGuiID dock_id_left_upper_right;
	ImGuiID dock_id_left_upper_left = ImGui::DockBuilderSplitNode(dock_id_left_upper, ImGuiDir_Left, 0.20f, NULL, &dock_id_left_upper_right);

	ImGui::DockBuilderDockWindow(hierarchy->GetWindowName().c_str(), dock_id_left_upper_left);
	ImGui::DockBuilderDockWindow(scene_panel->GetWindowName().c_str(), dock_id_left_upper_right);
	ImGui::DockBuilderDockWindow(game_panel->GetWindowName().c_str(), dock_id_left_upper_right);
	ImGui::DockBuilderDockWindow(inspector->GetWindowName().c_str(), dock_id_right);
	ImGui::DockBuilderDockWindow(console->GetWindowName().c_str(), dock_id_left_bottom);
	ImGui::DockBuilderDockWindow(project_explorer->GetWindowName().c_str(), dock_id_left_bottom);

	ImGui::DockBuilderFinish(editor_dockspace_id);
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	remove(TMP_SCENE_PATH);

	delete menu_bar;
	delete toolbar;
	for (auto& panel : panels)
	{
		delete panel;
	}

	return true;
}

void ModuleEditor::OpenScene(const std::string &path) const
{
	size_t readed_bytes;
	char* scene_file_data = App->filesystem->Load(path.c_str(), readed_bytes);
	std::string serialized_scene_string = scene_file_data;
	free(scene_file_data);

	Config scene_config(serialized_scene_string);
	App->scene->Load(scene_config);
}

void ModuleEditor::SaveScene(const std::string &path) const
{
	Config scene_config;
	App->resources->prefab_importer->Save(scene_config, App->scene->GetRoot());
	std::string serialized_scene_string;
	scene_config.GetSerializedString(serialized_scene_string);

	App->filesystem->Save(path.c_str(), serialized_scene_string.c_str(), serialized_scene_string.size() + 1);
}

ImFont* ModuleEditor::GetFont(const Fonts & font) const
{
	ImGuiIO& io = ImGui::GetIO();
	return io.Fonts->Fonts[static_cast<int>(font)];
}

void ModuleEditor::LoadFonts()
{
	ImGuiIO& io = ImGui::GetIO();

	// LOADING FONT AWESOME 5 (FONT_FA)
	io.Fonts->AddFontDefault();
	static const ImWchar icons_ranges_fa[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;

	io.Fonts->AddFontFromFileTTF("./resources/fonts/" FONT_ICON_FILE_NAME_FAS, 12.f, &icons_config, icons_ranges_fa);

	// LOADING FONT AWESOME 5 REGULAR (FONT_FAR)
	io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("./resources/fonts/" FONT_ICON_FILE_NAME_FAR, 12.f, &icons_config, icons_ranges_fa);

	// LOADING FONT AWESOME 5 BRANDS (FONT_FAB)
	io.Fonts->AddFontDefault();
	static const ImWchar icons_ranges_fab[] = { ICON_MIN_FAB, ICON_MAX_FAB, 0 };
	io.Fonts->AddFontFromFileTTF("./resources/fonts/" FONT_ICON_FILE_NAME_FAB, 12.f, &icons_config, icons_ranges_fab);
}

void ModuleEditor::ClearRedoStack()
{
	//TODO: delete all actions when engine closes (delete/add go/comp could end up in memory leak, be careful)
	while(!redoStack.empty())
	{
		EditorAction* action = redoStack.back();
		delete action;
		redoStack.pop_back();
	}
}

void ModuleEditor::ClearUndoStack()
{
	while (!undoStack.empty())
	{
		EditorAction* action = undoStack.back();
		delete action;
		undoStack.pop_back();
	}
}

void ModuleEditor::Undo()
{
	if(!undoStack.empty())
	{
		EditorAction* action = undoStack.back();
		action->Undo();

		redoStack.push_back(action);
		undoStack.pop_back();
	}
}

void ModuleEditor::Redo()
{
	if(!redoStack.empty())
	{
		EditorAction* action = redoStack.back();
		action->Redo();

		undoStack.push_back(action);
		redoStack.pop_back();
	}
}

void ModuleEditor::AddUndoAction(UndoActionType type)
{
	//StackUndoRedo set size maximum
	if(undoStack.size() >= MAXIMUM_SIZE_STACK_UNDO)
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
				selected_game_object->transform.GetTranslation(),
				selected_game_object
			);
			break;

		case UndoActionType::ROTATION:
			new_action = new EditorActionRotation(
				previous_transform,
				selected_game_object->transform.GetRotationRadiants(),
				selected_game_object
			);
			break;

		case UndoActionType::SCALE:
			new_action = new EditorActionScale(
				previous_transform,
				selected_game_object->transform.GetScale(),
				selected_game_object
			);
			break;

		case UndoActionType::ADD_GAMEOBJECT:
			new_action = new EditorActionAddGameObject(
				action_game_object,
				action_game_object->parent,
				action_game_object->GetHierarchyDepth()
			);
			break;

		case UndoActionType::DELETE_GAMEOBJECT:
			new_action = new EditorActionDeleteGameObject(
				action_game_object,
				action_game_object->parent,
				action_game_object->GetHierarchyDepth()
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

		case UndoActionType::EDIT_COMPONENTMATERIAL:
			new_action = new EditorActionSetTexture(
				(ComponentMaterial*)action_component,
				type_texture
			);
			break;

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

void ModuleEditor::DeleteComponentUndo(Component* component)
{
	//UndoRedo
	App->editor->action_component = component;
	App->editor->AddUndoAction(UndoActionType::DELETE_COMPONENT);
	component->Disable();
	auto it = std::find(component->owner->components.begin(), component->owner->components.end(), component);
	component->owner->components.erase(it);
}

void ModuleEditor::ClearUndoRedoStacks()
{
	ClearRedoStack();
	ClearUndoStack();
}