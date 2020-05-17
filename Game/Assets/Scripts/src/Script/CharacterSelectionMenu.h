#ifndef  __CHARATERSELECTIONMENU_H
#define  __CHARATERSELECTIONMENU_H

#include "Script.h"
class ComponentAudioSource;
class CharacterSelectionMenu : public Script
{

public:
	CharacterSelectionMenu();
	~CharacterSelectionMenu() = default;

	void Awake() override;
	void Update() override;

	void OnInspector(ImGuiContext * context) override;

	void InitPublicGameObjects() override;
private:
	std::vector<GameObject*> buttons;
	unsigned current = 0;
	bool selecting_character = false;

	bool awaked = false;
	bool enabled = false;

	GameObject* previous_panel = nullptr;
	GameObject* level_selection = nullptr;

	GameObject* back_button = nullptr;
	GameObject* level_selection_button = nullptr;

	GameObject* audio_controller = nullptr;
	ComponentAudioSource* audio_source = nullptr;

};
extern "C" SCRIPT_API CharacterSelectionMenu* CharacterSelectionMenuDLL(); //This is how we are going to load the script
#endif

