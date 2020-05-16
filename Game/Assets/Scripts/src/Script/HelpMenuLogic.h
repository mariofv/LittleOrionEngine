#ifndef  __HELPMENULOGIC_H__
#define  __HELPMENULOGIC_H__

#include "Script.h"
class ComponentAudioSource;
class HelpMenuLogic : public Script
{

public:
	HelpMenuLogic();
	~HelpMenuLogic() = default;
	
	void Awake() override;
	void Update() override;

	void OnInspector(ImGuiContext * context) override;

	void InitPublicGameObjects() override;
private:
	GameObject* help_joycon = nullptr;
	GameObject* help_keyboard = nullptr;
	GameObject* previous_panel = nullptr;
	bool enable = false;


	GameObject* audio_controller = nullptr;
	ComponentAudioSource* audio_source = nullptr;
};
extern "C" SCRIPT_API HelpMenuLogic* HelpMenuLogicDLL(); //This is how we are going to load the script
#endif
