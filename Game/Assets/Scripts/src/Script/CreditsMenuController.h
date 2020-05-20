#ifndef  __CREDITSMENUCONTROLLER_H__
#define  __CREDITSMENUCONTROLLER_H__

#include "Script.h"

class ComponentAudioSource;
class ComponentButton;
class MainMenuController;

class CreditsMenuController : public Script
{
public:
	CreditsMenuController();
	~CreditsMenuController() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	//void Save(Config& config) const override;
	//void Load(const Config& config) override;

	void Open();
	void Close();

private:
	GameObject* credits_panel = nullptr;

	GameObject* main_menu_game_object = nullptr;
	MainMenuController* main_menu_controller = nullptr;

	GameObject* back_buttton_game_object = nullptr;
	ComponentButton* back_button = nullptr;

	GameObject* audio_controller = nullptr;
	ComponentAudioSource* audio_source = nullptr;

	bool enabled = false;
	bool just_opened = false;

};
extern "C" SCRIPT_API CreditsMenuController* CreditsMenuControllerDLL(); //This is how we are going to load the script
#endif