#ifndef  __PLAYERMOVEMENT_H__
#define  __PLAYERMOVEMENT_H__

#include "Script.h"

#include "MathGeoLib.h"

class ComponentAnimation;
class ComponentCamera;
class ComponentCollider;
class WorldManager;

class PlayerMovement : public Script
{
public:
	PlayerMovement();
	~PlayerMovement() = default;

	void Awake() override;
	void Start() override;

	void OnInspector(ImGuiContext*) override;

	void Move(int player);
	void Jump(float3& direction);
	bool IsGrounded();

	bool IsInside(float3 transform);

	void InitPublicGameObjects();

public:
	ComponentCollider* collider = nullptr;

private:
	float speed = 0.2f;
	float jump_power = 130.0f;
	float second_jump_factor = -1000.f;

	bool is_grounded = false;
	bool is_second_jump = false;
	bool is_jumping = false;
	float current_y = 0.0f;

	ComponentAnimation* animation = nullptr;
	bool turning_back = false;

	GameObject* camera = nullptr;
	ComponentCamera* game_camera = nullptr;
	bool is_inside = true;
	bool visualize_future_aabb = false;

	GameObject* other_player = nullptr;
	WorldManager* world = nullptr;
	
	float3 direction = float3::zero;
	float3 distance = float3::zero;
	float3 velocity = float3::zero;
	float3 device_coordinates = float3::zero;
};
extern "C" SCRIPT_API PlayerMovement* PlayerMovementDLL(); //This is how we are going to load the script
#endif