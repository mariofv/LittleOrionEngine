#ifndef  __DAMAGEINDICATORSPAWNER_H__
#define  __DAMAGEINDICATORSPAWNER_H__

#include "Script.h"

#include <array>

class DamageIndicatorSpawner : public Script
{
public:
	DamageIndicatorSpawner();
	~DamageIndicatorSpawner() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	//void Save(Config& config) const override;
	//void Load(const Config& config) override;

	void SpawnDamageIndicator(int damage, float3 position);
	GameObject* GetAvailableDamageIndicator();

private:
	std::array<GameObject*, 5> damage_indicators_game_objects;

	GameObject* damage_indicator_game_object_0 = nullptr;
	GameObject* damage_indicator_game_object_1 = nullptr;
	GameObject* damage_indicator_game_object_2 = nullptr;
	GameObject* damage_indicator_game_object_3 = nullptr;
	GameObject* damage_indicator_game_object_4 = nullptr;
};
extern "C" SCRIPT_API DamageIndicatorSpawner* DamageIndicatorSpawnerDLL(); //This is how we are going to load the script
#endif