#ifndef  __EXAMPLESCRIPT_H__
#define  __EXAMPLESCRIPT_H__
#include "Script.h"
class ExampleScript : public Script
{
	void Update(GameObject*);
	void OnInspector();
};
extern "C" SCRIPT_API ExampleScript* ExampleScriptDLL(); //This is how we are going to load the script
#endif