#ifndef  __PLAYERMOVEMENT_H__
#define  __PLAYERMOVEMENT_H__

#include "Script.h"

#include "MathGeoLib.h"

class ComponentAnimation;
class ComponentCamera;
class ComponentCollider;
class ComponentAudioSource;

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
	float rotation_speed = 0.01f;
	float falling_factor = 1.0f;
	bool is_jumping = false;
	float current_y = 0.0f;
	float jump_power = 130.0f;
	float3 movement_vector;
	float3 gravity_vector;

	ComponentAudioSource* audio_source = nullptr;
	ComponentAnimation* animation = nullptr;

	GameObject* camera = nullptr;
	ComponentCamera* game_camera = nullptr;
	bool is_inside = true;
	bool is_grounded = false;
	bool is_second_jump = false;
	bool visualize_future_aabb = false;
	bool turning_back = false;

	float3 direction = float3::zero;
	float3 distance = float3::zero;
	GameObject* second_player = nullptr;
	float3 velocity = float3::zero;

	//TODO:uncomment next line
	//float next_step_percentage = 0.21f;
};
extern "C" SCRIPT_API PlayerMovement* PlayerMovementDLL(); //This is how we are going to load the script
#endif
