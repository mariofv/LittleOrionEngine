#ifndef  __TRIGGERGRAVITY_H__
#define  __TRIGGERGRAVITY_H__

#include "Script.h"

class PlayerController;

class TriggerGravity : public Script
{
public:
	TriggerGravity();
	~TriggerGravity() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	bool OnTriggerEnter() const;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	void Save(Config& config) const override;
	void Load(const Config& config) override;

public:
	bool up_down = false;

private:
	GameObject* player = nullptr;
	PlayerController* movement_script = nullptr;
	ComponentScript* player_movement_component = nullptr;

};
extern "C" SCRIPT_API TriggerGravity* TriggerGravityDLL(); //This is how we are going to load the script
#endif