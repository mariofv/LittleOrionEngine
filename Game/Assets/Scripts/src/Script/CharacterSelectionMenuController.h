#ifndef  __CHARATERSELECTIONMENUCONTROLLER_H
#define  __CHARATERSELECTIONMENUCONTROLLER_H

#include "Script.h"

class ComponentAudioSource;

class LevelSelectionMenuController;
class MainMenuController;

class CharacterSelectionMenuController : public Script
{

public:
	CharacterSelectionMenuController();
	~CharacterSelectionMenuController() = default;

	void Awake() override;
	void Update() override;

	void OnInspector(ImGuiContext * context) override;
	void InitPublicGameObjects() override;

	void SelectCharacter();
	void OpenSubMenu(int index);

	void Open();
	void Close();

private:
	std::vector<GameObject*> buttons;
	unsigned current = 0;

	bool player1_choice = true; //TODO: CHANGE THIS FOR THE WORLDMANAGER VARIABLE
	bool selecting_character = false;

	bool enabled = false;
	bool just_opened = false;

	GameObject* character_selection_panel = nullptr;

	GameObject* main_menu_game_object = nullptr;
	MainMenuController* main_menu_controller = nullptr;

	GameObject* level_selection_game_object = nullptr;
	LevelSelectionMenuController* level_selection_controller = nullptr;

	GameObject* p1_position = nullptr;
	GameObject* p2_position = nullptr;

	GameObject* character_selector1= nullptr;
	GameObject* character_selector2 = nullptr;

	GameObject* confirm_multiplayer = nullptr;
	GameObject* confirm_singleplayer = nullptr;

	GameObject* back_button = nullptr;
	GameObject* level_selection_button = nullptr;

	GameObject* audio_controller = nullptr;
	ComponentAudioSource* audio_source = nullptr;

};
extern "C" SCRIPT_API CharacterSelectionMenuController* CharacterSelectionMenuControllerDLL(); //This is how we are going to load the script
#endif

