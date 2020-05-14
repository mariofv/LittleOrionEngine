#ifndef  __MENULOGIC_H__
#define  __MENULOGIC_H__

#include "Script.h"

class ComponentAudioSource;
class MenuLogic : public Script
{
public:
	MenuLogic();
	~MenuLogic() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	//void Save(Config& config) const override;
	//void Load(const Config& config) override;

private:
	bool ComfirmButtonPressed();

private:
	GameObject* play_button = nullptr;
	GameObject* help_button = nullptr;
	GameObject* credits_button = nullptr;
	GameObject* exit_button = nullptr;
	GameObject* cursor = nullptr;

	GameObject* background = nullptr;
	GameObject* help_controller = nullptr;
	GameObject* help_keyboard = nullptr;
	GameObject* credits_panel = nullptr;
	GameObject* audio_controller = nullptr;


	ComponentAudioSource* audio_source = nullptr;
	std::vector<GameObject*> buttons;
	unsigned current = 0;
	
	bool show_help = false;
	bool show_credits = false;

};
extern "C" SCRIPT_API MenuLogic* MenuLogicDLL(); //This is how we are going to load the script
#endif