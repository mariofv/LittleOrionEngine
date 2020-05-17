#ifndef  __PLAYERCONTROLLER_H__
#define  __PLAYERCONTROLLER_H__

#include "Script.h"

class ComponentCollider;
class PlayerMovement;
class PlayerAttack;
class DebugModeScript;

class PlayerController : public Script
{
public:
	PlayerController();
	~PlayerController() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void TakeDamage(float damage);
	ComponentCollider* GetCollider();
	void InitPublicGameObjects() override;

public:
	bool on_gravity = false;
	unsigned int player = 1;
	bool is_alive = true;
	GameObject* debug_system = nullptr;

private:
	PlayerMovement* player_movement = nullptr;
	PlayerAttack* player_attack = nullptr;
	DebugModeScript* debug = nullptr;
	//unsigned player = 0;
	float health_points = 100.0f;
};
extern "C" SCRIPT_API PlayerController* PlayerControllerDLL(); //This is how we are going to load the script
#endif