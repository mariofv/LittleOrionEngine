#ifndef  __DAMAGEINDICATOR_H__
#define  __DAMAGEINDICATOR_H__

#include "Script.h"

class ComponentText;

class DamageIndicator : public Script
{
public:
	DamageIndicator();
	~DamageIndicator() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	void Save(Config& config) const override;
	void Load(const Config& config) override;

	bool IsAlive() const;
	void Spawn(int number, float2 position);

private:
	ComponentText* damage_indicator_text = nullptr;

	bool alive = false;
	float speed = 0.5f;
	
	float duration = 1000.f;
	float current_time = 0.f;

	float offset_y = 40.f;
};
extern "C" SCRIPT_API DamageIndicator* DamageIndicatorDLL(); //This is how we are going to load the script
#endif