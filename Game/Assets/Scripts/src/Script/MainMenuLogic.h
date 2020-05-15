#ifndef  __MAINMENULOGIC_H__
#define  __MAINMENULOGIC_H__

#include "Script.h"

class ComponentAudioSource;
class ComponentTransform2D;
class MainMenuLogic : public Script
{
public:
	MainMenuLogic();
	~MainMenuLogic() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects() override;

private:
	bool ConfirmMovedUp();
	bool ConfirmMovedDown();
	bool ComfirmButtonPressed();
	//void Save(Config& config) const override;
	//void Load(const Config& config) override;

private:
	GameObject* play_button = nullptr;
	GameObject* help_button = nullptr;
	GameObject* credits_button = nullptr;
	GameObject* exit_button = nullptr;
	GameObject* cursor = nullptr;

	GameObject* credits_panel = nullptr;
	GameObject* help_panel = nullptr;
	GameObject* audio_controller = nullptr;


	ComponentAudioSource* audio_source = nullptr;
	std::vector<GameObject*> buttons;
	ComponentTransform2D* credits_back_button = nullptr;
	unsigned current = 0;
	
	bool show_help = false;
	bool show_credits = false;

};
extern "C" SCRIPT_API MainMenuLogic* MainMenuLogicDLL(); //This is how we are going to load the script
#endif