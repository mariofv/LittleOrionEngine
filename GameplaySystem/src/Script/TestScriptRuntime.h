#ifndef  __TESTSCRIPTRUNTIME_H__
#define  __TESTSCRIPTRUNTIME_H__

#include "Script.h"

class TestScriptRuntime : public Script
{
public:
	TestScriptRuntime();
	~TestScriptRuntime() = default;
	void Update() override;
	void OnInspector(ImGuiContext*) override;
	void Test();

	float speed = 0.50f;
	float rotation_speed = 0.01f;
};
extern "C" SCRIPT_API TestScriptRuntime* TestScriptRuntimeDLL(); //This is how we are going to load the script
#endif

