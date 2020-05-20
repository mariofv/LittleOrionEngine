#ifndef  __PLAYERCONTROLLER_H__
#define  __PLAYERCONTROLLER_H__

#include "Script.h"

class ComponentCollider;
class DebugModeScript;
class PlayerMovement;
class PlayerAttack;
class ProgressBar;

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
	void MakePlayerFall(float3& direction) const;

	void InitPublicGameObjects() override;

public:
	unsigned int player = 1;
	bool is_alive = true;
private:
	PlayerMovement* player_movement = nullptr;
	PlayerAttack* player_attack = nullptr;

	GameObject* progress_bar = nullptr;
	ProgressBar* health_bar = nullptr;
	DebugModeScript* debug = nullptr;
	//unsigned player = 0;
	float total_health = 1000.f;
	float health_points = 1000.f;
};
extern "C" SCRIPT_API PlayerController* PlayerControllerDLL(); //This is how we are going to load the script
#endif
