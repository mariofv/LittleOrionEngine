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
	void Jump();
	bool IsGrounded();

	bool IsInside(float3 transform);

	void InitPublicGameObjects();

private:
	float speed = 0.2f;
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
	bool is_grounded = false;

	float3 new_translation;
	GameObject* second_player = nullptr;
};
extern "C" SCRIPT_API PlayerMovement* PlayerMovementDLL(); //This is how we are going to load the script
#endif