#ifndef  __EXAMPLESCRIPT_H__
#define  __EXAMPLESCRIPT_H__
#include "Script.h"


class ExampleScript : public Script
{
public:
	ExampleScript();
	~ExampleScript() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;

	void Test();

	void InitPublicGameObjects();

	void Save(Config& config) const override;
	void Load(const Config& config) override;

public:
	GameObject* example = nullptr;
	ComponentScript* example_component = nullptr;
	float speed = 0.01f;
	float rotation_speed = 0.01f;

};
extern "C" SCRIPT_API ExampleScript* ExampleScriptDLL(); //This is how we are going to load the script
#endif