#ifndef  __ATTACKENEMYSTATE_H__
#define  __ATTACKENEMYSTATE_H__

#include "Script.h"

class AttackEnemyState : public Script
{
public:
	AttackEnemyState();
	~AttackEnemyState() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	//void Save(Config& config) const override;
	//void Load(const Config& config) override;

private:
	GameObject* example = nullptr;

};
extern "C" SCRIPT_API AttackEnemyState* AttackEnemyStateDLL(); //This is how we are going to load the script
#endif