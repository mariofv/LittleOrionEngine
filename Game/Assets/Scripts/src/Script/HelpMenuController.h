#ifndef  __HELPMENUCONTROLLER_H__
#define  __HELPMENUCONTROLLER_H__

#include "Script.h"

class ComponentAudioSource;
class ComponentButton;
class MainMenuController;

class HelpMenuController : public Script
{

public:
	HelpMenuController();
	~HelpMenuController() = default;
	
	void Awake() override;
	void Update() override;

	void OnInspector(ImGuiContext * context) override;

	void InitPublicGameObjects() override;
	void Open();
	void Close();

private:
	GameObject* main_menu_panel = nullptr;
	MainMenuController* main_menu_controller = nullptr;
	GameObject* help_menu_panel = nullptr;

	GameObject* help_joycon_background = nullptr;
	GameObject* help_keyboard_background = nullptr;

	GameObject* help_joycon_button_game_object = nullptr;
	ComponentButton* help_joycon_button = nullptr;

	GameObject* help_keyboard_button_game_object = nullptr;
	ComponentButton* help_keyboard_button = nullptr;

	GameObject* back_button_game_object = nullptr;
	ComponentButton* back_button = nullptr;

	GameObject* audio_controller = nullptr;
	ComponentAudioSource* audio_source = nullptr;

	bool enabled = false;
	bool just_opened = false;
};
extern "C" SCRIPT_API HelpMenuController* HelpMenuControllerDLL(); //This is how we are going to load the script
#endif
