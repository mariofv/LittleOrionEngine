#ifndef  __ENEMYMANAGER_H__
#define  __ENEMYMANAGER_H__

#include "Script.h"

class EventManager;
class EnemyController;

const unsigned MAX_NUMBER_OF_MUSHDOOM = 10;
const float3 graveyard_position(100.f,100.f,100.f);

class EnemyManager : public Script
{
public:
	EnemyManager();
	~EnemyManager() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void AddEnemy(EnemyController* enemy);
	void KillEnemy(EnemyController* enemy);

	void SpawnEnemy(const unsigned type, const float3& spawn_position);
	void SpawnWave(unsigned event, unsigned enemies_per_wave);
	void CreateEnemies();

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	//void Save(Config& config) const override;
	//void Load(const Config& config) override;

private:
	void InitSpawnPoints();
	bool CheckSpawnAvailability(float3& spawn_position);

public:
	std::vector<EnemyController*> enemies;
	unsigned current_number_of_enemies_alive = 0;


private:
	//We need a reference to an existing mushdoom to duplicate_him
	GameObject* mushdoom_go = nullptr;
	EventManager* event_manager = nullptr;
	std::vector<float3> enemies_spawning_queue;
	unsigned total_enemies_killed = 0;
	float3 spawn_points[5];
};
extern "C" SCRIPT_API EnemyManager* EnemyManagerDLL(); //This is how we are going to load the script
#endif