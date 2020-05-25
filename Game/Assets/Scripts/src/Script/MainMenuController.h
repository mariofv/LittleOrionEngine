#ifndef  __MAINMENUCONTROLLER_H__
#define  __MAINMENUCONTROLLER_H__

#include "Script.h"

class ComponentAudioSource;
class ComponentButton;
class ComponentTransform2D;

class CharacterSelectionMenuController;
class CreditsMenuController;
class HelpMenuController;

class MainMenuController : public Script
{
public:
	MainMenuController();
	~MainMenuController() = default;

	void Awake() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects() override;

	void OpenSubMenu(int menu_index);

	void Open();
	void Close();

private:
	GameObject* play_button = nullptr;
	GameObject* help_button = nullptr;
	GameObject* credits_button = nullptr;
	GameObject* exit_button = nullptr;
	GameObject* cursor = nullptr;

	GameObject* main_menu_panel = nullptr;

	GameObject* character_selection_game_object = nullptr;
	CharacterSelectionMenuController* character_selection_controller = nullptr;

	GameObject* credits_game_object = nullptr;
	CreditsMenuController* credits_controller = nullptr;
	
	GameObject* help_game_object = nullptr;
	HelpMenuController* help_controller = nullptr;
	
	GameObject* audio_controller = nullptr;
	ComponentAudioSource* audio_source = nullptr;

	std::vector<ComponentTransform2D*> buttons_transforms;
	std::vector<ComponentButton*> buttons_components;
	unsigned current_highlighted_button = 0;

	bool enabled = true;
	bool just_opened = false;
	bool cursor_initialized = false;
};
extern "C" SCRIPT_API MainMenuController* MainMenuControllerDLL(); //This is how we are going to load the script
#endif