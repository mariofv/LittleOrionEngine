#ifndef  __PLAYERMOVEMENT_H__
#define  __PLAYERMOVEMENT_H__

#include "Script.h"

class PlayerMovement : public Script
{
public:
	PlayerMovement();
	~PlayerMovement() = default;

	void Awake() override;
	void Start() override;

	void OnInspector(ImGuiContext*) override;

	void Move(int player);
	void Dash();

	void Save(Config& config) const override;
	void Load(const Config& config) override;
	void Link() override;

private:
	float speed = 0.5f;
	float rotation_speed = 0.01f;
	bool is_jumping = false;
	float current_y = 0.0f;
	float jump_power = 10.0f;
	float3 movement_vector;
	float3 gravity_vector;

};
extern "C" SCRIPT_API PlayerMovement* PlayerMovementDLL(); //This is how we are going to load the script
#endif