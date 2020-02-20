#ifndef  __SCRIPT_H__
#define  __SCRIPT_H__
#ifdef SCRIPT_EXPORTS
#define SCRIPT_API __declspec(dllexport)
#else
#define SCRIPT_API __declspec(dllimport)
#endif
class GameObject;
class Script 
{
public:
	virtual void Update(GameObject*) {}
	virtual void OnInspector() {}
};
extern "C" SCRIPT_API Script* ScriptDLL();
typedef Script* (*CREATE_SCRIPT) ();
#endif