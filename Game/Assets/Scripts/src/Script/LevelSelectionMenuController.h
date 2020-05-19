#ifndef  _LEVELSELECTIONMENUCONTROLLER_H_
#define  _LEVELSELECTIONMENUCONTROLLER_H_

#include "Script.h"

class ComponentAudioSource;
class MainMenuController;

class LevelSelectionMenuController : public Script
{

public:
	LevelSelectionMenuController();
	~LevelSelectionMenuController() = default;

	void Awake() override;
	void Update() override;

	void OnInspector(ImGuiContext * context) override;
	void InitPublicGameObjects() override;

	void Open();
	void Close();

private:
	std::vector<GameObject*> buttons;
	unsigned current = 0;

	bool selecting_level = false;
	bool enabled = false;
	bool just_opened = false;

	GameObject* level_selection_panel = nullptr;

	GameObject* main_menu_game_object = nullptr;
	MainMenuController* main_menu_controller = nullptr;

	GameObject* level1 = nullptr;
	GameObject* level2 = nullptr;
	GameObject* level3 = nullptr;
	GameObject* back = nullptr;
	GameObject* back_cursor = nullptr;

	GameObject* audio_controller = nullptr;
	ComponentAudioSource* audio_source = nullptr;

	const size_t LEVEL1_POSITION = 0;
	const size_t LEVEL2_POSITION = 0;
	const size_t LEVEL3_POSITION = 0;

};
extern "C" SCRIPT_API LevelSelectionMenuController* LevelSelectionMenuControllerDLL(); //This is how we are going to load the script
#endif


