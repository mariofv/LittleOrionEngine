#ifndef  __IDLEENEMYSTATE_H__
#define  __IDLEENEMYSTATE_H__

#include "Script.h"

class IdleEnemyState : public Script
{
public:
	IdleEnemyState();
	~IdleEnemyState() = default;

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
extern "C" SCRIPT_API IdleEnemyState* IdleEnemyStateDLL(); //This is how we are going to load the script
#endif