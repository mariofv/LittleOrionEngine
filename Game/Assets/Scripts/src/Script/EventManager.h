#ifndef  __EVENTMANAGER_H__
#define  __EVENTMANAGER_H__

#include "Script.h"
#include "EnemyManager.h"
#include "CameraController.h"

class EventManager : public Script
{
public:
	EventManager();
	~EventManager() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	bool TriggerEvent(unsigned event);


public:
	unsigned enemies_killed_on_wave = 0;
	bool event_triggered = false;

private:
	EnemyManager* enemy_manager = nullptr;
	CameraController* camera_controller = nullptr;
	unsigned waves_left = 0;
	unsigned enemies_per_wave = 0;
	unsigned current_event = 0;

};
extern "C" SCRIPT_API EventManager* EventManagerDLL(); //This is how we are going to load the script
#endif