#ifndef  __PLAYERATTACK_H__
#define  __PLAYERATTACK_H__

#include "Script.h"

class ComponentAnimation;

const unsigned PUNCH_DAMAGE = 33;
const unsigned KICK_DAMAGE = 45;

class PlayerAttack : public Script
{
public:
	PlayerAttack();
	~PlayerAttack() = default;


	void Awake() override;
	void Start() override;
	bool Attack();

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();

	//void Save(Config& config) const override;
	//void Load(const Config& config) override;

private:
	ComponentAnimation* animation = nullptr;
	bool is_attacking = false;
};

extern "C" SCRIPT_API PlayerAttack* PlayerAttackDLL(); //This is how we are going to load the script

#endif