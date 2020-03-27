#ifndef  __TRIGGERACTIONSCRIPT_H__
#define  __TRIGGERACTIONSCRIPT_H__

#include "Script.h"

#include <MathGeoLib/MathGeoLib.h>

class PlayerController;

class TriggerActionScript : public Script
{
public:
	TriggerActionScript();
	~TriggerActionScript() = default;
	void Awake() override;
	void Start() override;
	void Update() override;
	void OnInspector(ImGuiContext*) override;
	bool OnTriggerEnter() const;
	void InitPublicGameObjects() override;

public:
	GameObject* trigger_go = nullptr;
	GameObject* player = nullptr;
	PlayerController* movement_script = nullptr;
	ComponentScript* movement_component = nullptr;


private:
	float3 start_position;
};
extern "C" SCRIPT_API TriggerActionScript* TriggerActionScriptDLL(); //This is how we are going to load the script
#endif