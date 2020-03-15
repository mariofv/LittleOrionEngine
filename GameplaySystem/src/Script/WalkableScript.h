#ifndef  __WALKABLESCRIPT_H__
#define  __WALKABLESCRIPT_H__

#include "Script.h"

class WalkableScript : public Script
{
public:
	WalkableScript();
	~WalkableScript() = default;
	void Awake() override;
	void Start() override;
	void Update() override;
	void OnInspector(ImGuiContext*) override;
	void Move();

private:
	PanelComponent* panel = nullptr;
	float speed = 0.01f;
	float rotation_speed = 0.01f;

};
extern "C" SCRIPT_API WalkableScript* WalkableScriptDLL(); //This is how we are going to load the script
#endif