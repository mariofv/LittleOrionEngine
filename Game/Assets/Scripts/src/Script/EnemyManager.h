#ifndef  __ENEMYMANAGER_H__
#define  __ENEMYMANAGER_H__

#include "Script.h"

class EnemyController;

const unsigned MAX_NUMBER_OF_MUSHDOOM = 100;
const float3 graveyard_position(100.f,100.f,100.f);

class EnemyManager : public Script
{
public:
	EnemyManager();
	~EnemyManager() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	static EnemyManager* GetInstance();

	void AddEnemy(EnemyController* enemy);
	void KillEnemy(EnemyController* enemy);

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	//void Save(Config& config) const override;
	//void Load(const Config& config) override;

private:
	GameObject* enemy_go = nullptr;

	
	std::vector<EnemyController*> enemies;
	static EnemyManager* instance_singleton;

	unsigned int current_number_of_enemies_alive = 0;


};
extern "C" SCRIPT_API EnemyManager* EnemyManagerDLL(); //This is how we are going to load the script
#endif