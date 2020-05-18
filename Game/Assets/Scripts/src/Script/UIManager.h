#ifndef  __UIMANAGER_H__
#define  __UIMANAGER_H__

#include "Script.h"

class DamageIndicatorSpawner;
class ProgressBar;

class UIManager : public Script
{
public:
	UIManager();
	~UIManager() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	//void Save(Config& config) const override;
	//void Load(const Config& config) override;

	void SetPlayer1Health(float percentage);
	void SetPlayer2Health(float percentage);

	void SpawnDamageIndicator(int damage, float3 position);

private:
	GameObject* player1_progress_bar_game_object = nullptr;
	ProgressBar* player1_progress_bar = nullptr;

	GameObject* player2_progress_bar_game_object = nullptr;
	ProgressBar* player2_progress_bar = nullptr;

	GameObject* damage_indicator_spawner_game_object = nullptr;
	DamageIndicatorSpawner* damage_indicator_spawner = nullptr;
};
extern "C" SCRIPT_API UIManager* UIManagerDLL(); //This is how we are going to load the script
#endif