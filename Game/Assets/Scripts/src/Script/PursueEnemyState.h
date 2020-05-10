#ifndef  __PURSUEENEMYSTATE_H__
#define  __PURSUEENEMYSTATE_H__

#include "Script.h"

class PursueEnemyState : public Script
{
public:
	PursueEnemyState();
	~PursueEnemyState() = default;

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
extern "C" SCRIPT_API PursueEnemyState* PursueEnemyStateDLL(); //This is how we are going to load the script
#endif