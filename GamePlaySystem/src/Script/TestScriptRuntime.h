#ifndef  __TESTSCRIPTRUNTIME_H__
#define  __TESTSCRIPTRUNTIME_H__
#include "Script.h"

class TestScriptRuntime : public Script
{
public:
	TestScriptRuntime();
	~TestScriptRuntime();
	void Update() override;
	void OnInspector(ImGuiContext*) override;
	void Test();

	int i = 0;
	float speed = 0.01f;
	float rotation_speed = 0.01f;
};
extern "C" SCRIPT_API TestScriptRuntime* TestScriptRuntimeDLL(); //This is how we are going to load the script
#endif

