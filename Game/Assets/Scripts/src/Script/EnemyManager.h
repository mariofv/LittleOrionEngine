#ifndef  __ENEMYMANAGER_H__
#define  __ENEMYMANAGER_H__

#include "Script.h"

class EnemyController;

const unsigned MAX_NUMBER_OF_MUSHDOOM = 10;
const float3 graveyard_position(100.f,100.f,100.f);
const float3 spawn_points[3]{float3(-84.310f,-1.462f,0.894f),float3(-53.853f,6.564f,0.894f),float3(-38.499f,6.564f,0.894f)};

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

	void CreateEnemies();

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	//void Save(Config& config) const override;
	//void Load(const Config& config) override;

public:
	std::vector<EnemyController*> enemies;


private:
	//We need a reference to an existing mushdoom to duplicate_him
	GameObject* mushdoom_go = nullptr;

	unsigned current_number_of_enemies_alive = 0;
	unsigned max_enemies_alive = 2;

};
extern "C" SCRIPT_API EnemyManager* EnemyManagerDLL(); //This is how we are going to load the script
#endif