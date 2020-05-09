#ifndef  __PLAYERMOVEMENT_H__
#define  __PLAYERMOVEMENT_H__

#include "Script.h"

#include "MathGeoLib.h"

class ComponentCamera;
class ComponentCollider;

class PlayerMovement : public Script
{
public:
	PlayerMovement();
	~PlayerMovement() = default;

	void Awake() override;
	void Start() override;

	void OnInspector(ImGuiContext*) override;

	void Move(int player);
	void Fall();
	void Dash();

	bool IsInside(float3 transform);

	void InitPublicGameObjects();

private:
	float speed = 35.f;
	//float speed = 5.0F;
	float rotation_speed = 0.01f;
	float falling_factor = 1.0f;
	bool is_jumping = false;
	float current_y = 0.0f;
	float jump_power = 10.0f;
	float3 movement_vector;
	float3 gravity_vector;
	ComponentCollider* collider = nullptr;

	GameObject* camera = nullptr;
	ComponentCamera* game_camera = nullptr;
	bool is_inside = true;

};
extern "C" SCRIPT_API PlayerMovement* PlayerMovementDLL(); //This is how we are going to load the script
#endif