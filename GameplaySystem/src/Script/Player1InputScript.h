#ifndef  __PLAYER1INPUTSCRIPT_H__
#define  __PLAYER1INPUTSCRIPT_H__

#include "Script.h"

class Player1InputScript : public Script
{
public:
	Player1InputScript();
	~Player1InputScript() = default;
	void Awake() override;
	void Start() override;
	void Update() override;
	void OnInspector(ImGuiContext*) override;

private:
	//PanelComponent* panel = nullptr;
	float speed = 0.5f;

};
extern "C" SCRIPT_API Player1InputScript* Player1InputScriptDLL(); //This is how we are going to load the script
#endif