#ifndef  __SCREAMENEMYSTATE_H__
#define  __SCREAMENEMYSTATE_H__

#include "Script.h"

class ScreamEnemyState : public Script
{
public:
	ScreamEnemyState();
	~ScreamEnemyState() = default;

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
extern "C" SCRIPT_API ScreamEnemyState* ScreamEnemyStateDLL(); //This is how we are going to load the script
#endif