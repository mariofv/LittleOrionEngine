#ifndef  __ENEMYSTATE_H__
#define  __ENEMYSTATE_H__

#include "Script.h"

class EnemyState : public Script
{
public:
	EnemyState();
	~EnemyState() = default;

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
extern "C" SCRIPT_API EnemyState* EnemyStateDLL(); //This is how we are going to load the script
#endif