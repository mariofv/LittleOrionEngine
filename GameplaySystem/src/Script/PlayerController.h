#ifndef  __PLAYERCONTROLLER_H__
#define  __PLAYERCONTROLLER_H__

#include "Script.h"

class PlayerController : public Script
{
public:
	PlayerController();
	~PlayerController() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;
	void Link() override;

private:
	PanelComponent* panel = nullptr;
	GameObject* example = nullptr;
	uint64_t exampleUUID = -1;

};
extern "C" SCRIPT_API PlayerController* PlayerControllerDLL(); //This is how we are going to load the script
#endif