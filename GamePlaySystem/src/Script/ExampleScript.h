#ifndef  __EXAMPLESCRIPT_H__
#define  __EXAMPLESCRIPT_H__
#include "Script.h"
class ExampleScript : public Script
{
public:
	ExampleScript();
	~ExampleScript();
	void Update() override;
	void OnInspector(ImGuiContext*) override;
	void Test();
	int i = 0;
	float speed = 0.01f;
};
extern "C" SCRIPT_API ExampleScript* ExampleScriptDLL(); //This is how we are going to load the script
#endif