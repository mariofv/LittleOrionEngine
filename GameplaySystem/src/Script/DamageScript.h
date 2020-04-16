#ifndef  __DAMAGESCRIPT_H__
#define  __DAMAGESCRIPT_H__

#include "Script.h"

class ComponentProgressBar;

class DamageScript : public Script
{
public:
	DamageScript();
	~DamageScript() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	bool OnTriggerEnter() const;
	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	//void Save(Config& config) const override;
	//void Load(const Config& config) override;

private:
	GameObject* player = nullptr;
	GameObject* health_bar = nullptr;
	ComponentProgressBar* health_component = nullptr;
	float damage = 20.0f;

};
extern "C" SCRIPT_API DamageScript* DamageScriptDLL(); //This is how we are going to load the script
#endif