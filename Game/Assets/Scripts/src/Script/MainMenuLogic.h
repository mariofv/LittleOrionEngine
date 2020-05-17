#ifndef  __MAINMENULOGIC_H__
#define  __MAINMENULOGIC_H__

#include "Script.h"
#include "MenuController.h"

class ComponentAudioSource;
class ComponentTransform2D;
class MainMenuLogic : public Script
{
public:
	MainMenuLogic();
	~MainMenuLogic() = default;

	void Awake() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects() override;

private:
	GameObject* play_button = nullptr;
	GameObject* help_button = nullptr;
	GameObject* credits_button = nullptr;
	GameObject* exit_button = nullptr;
	GameObject* cursor = nullptr;

	GameObject* character_selection_panel = nullptr;
	GameObject* credits_panel = nullptr;
	GameObject* help_panel = nullptr;
	GameObject* audio_controller = nullptr;


	ComponentAudioSource* audio_source = nullptr;
	ComponentTransform2D* credits_back_button = nullptr;
	std::vector<GameObject*> buttons;
	unsigned current = 0;
	
	bool show_credits = false;

};
extern "C" SCRIPT_API MainMenuLogic* MainMenuLogicDLL(); //This is how we are going to load the script
#endif