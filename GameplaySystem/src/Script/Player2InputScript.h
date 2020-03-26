#ifndef  __PLAYER2INPUTSCRIPT_H__
#define  __PLAYER2INPUTSCRIPT_H__

#include "Script.h"

class Player2InputScript : public Script
{
public:
	Player2InputScript();
	~Player2InputScript() = default;
	void Awake() override;
	void Start() override;
	void Update() override;
	void OnInspector(ImGuiContext*) override;

private:
	float speed = 0.5f;
};
extern "C" SCRIPT_API Player2InputScript* Player2InputScriptDLL(); //This is how we are going to load the script
#endif