#ifndef  __WORLDMANAGER_H__
#define  __WORLDMANAGER_H__

#include "Script.h"

class ComponentProgressBar;
class ComponentImage;


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
	GameObject* end_level = nullptr;
	GameObject* health_bar = nullptr;
	GameObject* lose_screen = nullptr;
	GameObject* win_screen = nullptr;
	GameObject* player = nullptr;
	ComponentImage* lose_component = nullptr;
	ComponentImage* win_component = nullptr;
	ComponentProgressBar* health_component = nullptr;
	ComponentScript* player_controller = nullptr;

	bool transition = false;

};
extern "C" SCRIPT_API WorldManager* WorldManagerDLL(); //This is how we are going to load the script
#endif