#ifndef  __WORLDMANAGER_H__
#define  __WORLDMANAGER_H__

#include "Script.h"
#include "EventManager.h"

class ComponentCollider;


class WorldManager : public Script
{
public:
	WorldManager();
	~WorldManager() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	bool OnTriggerEnter() const;
	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();

	//void Save(Config& config) const override;
	//void Load(const Config& config) override;

private:
	void InitTriggers();
	void CheckTriggers();

private:
	GameObject* health_bar = nullptr;
	GameObject* lose_screen = nullptr;
	GameObject* win_screen = nullptr;
	GameObject* player = nullptr;
	
	ComponentScript* player_controller = nullptr;
	EventManager* event_manager = nullptr;

	ComponentCollider* event_triggers[3];
	unsigned current_event_trigger = 0;
	bool transition = false;

};
extern "C" SCRIPT_API WorldManager* WorldManagerDLL(); //This is how we are going to load the script
#endif