#ifndef  __PAUSEMENUCONTROLLER_H__
#define  __PAUSEMENUCONTROLLER_H__

#include "Script.h"

class ComponentAudioSource;

class PauseMenuController : public Script
{

public:
	PauseMenuController();
	~PauseMenuController() = default;

	void Awake() override;
	void Update() override;

	void OnInspector(ImGuiContext * context) override;

	void InitPublicGameObjects() override;
private:
	std::vector<GameObject*> buttons;
	unsigned current = 0;
	bool game_paused = false;
	float time_scale = 0.0f;
	bool awaked = false;
	GameObject* resume_button = nullptr;
	GameObject* help_button = nullptr;
	//GameObject* level_selection_button = nullptr;
	GameObject* main_menu_button = nullptr;
	GameObject* help_panel = nullptr;

	GameObject* audio_controller = nullptr;
	ComponentAudioSource* audio_source = nullptr;

	const size_t MAIN_MENU_SCENE = 1;
};
extern "C" SCRIPT_API PauseMenuController* PauseMenuControllerDLL(); //This is how we are going to load the script
#endif