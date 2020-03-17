#ifndef  __ENDOFLEVELSCRIPT_H__
#define  __ENDOFLEVELSCRIPT_H__

#include "Script.h"
#include "Main/Globals.h"

class EndOfLevelScript : public Script
{
public:
	EndOfLevelScript();
	~EndOfLevelScript() = default;
	void Awake() override;
	void Start() override;
	void Update() override;
	void OnInspector(ImGuiContext*) override;

	bool OnTriggerEnter();

public:
	GameObject* trigger_go = nullptr;

private:
	PanelComponent* panel = nullptr;
	std::string is_object = "None";
	std::string level_to_load_path = DEFAULT_SCENE_PATH;
	bool entered = false;

};
extern "C" SCRIPT_API EndOfLevelScript* EndOfLevelScriptDLL(); //This is how we are going to load the script
#endif