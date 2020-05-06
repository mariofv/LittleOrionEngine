#ifndef  __ENEMYMANAGER_H__
#define  __ENEMYMANAGER_H__

#include "Script.h"

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

	void CreateEnemies();

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	//void Save(Config& config) const override;
	//void Load(const Config& config) override;

private:
	//We need a reference to an existing mushdoom to duplicate_him
	GameObject* mushdoom_go = nullptr;
	std::vector<EnemyController*> enemies;

	unsigned int current_number_of_enemies_alive = 0;
	bool enemies_instantiated = false;

};
extern "C" SCRIPT_API EnemyManager* EnemyManagerDLL(); //This is how we are going to load the script
#endif