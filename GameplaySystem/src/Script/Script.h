#ifndef  __SCRIPT_H__
#define  __SCRIPT_H__
#ifdef SCRIPT_EXPORTS
#define SCRIPT_API __declspec(dllexport)
#else
#define SCRIPT_API __declspec(dllimport)
#endif
class GameObject;
class Application;
struct ImGuiContext;

class Script 
{
public:
	Script();
	~Script();

	virtual void Update() {};
	virtual void OnInspector(ImGuiContext*) {};
	virtual void AddReferences(GameObject* owner, Application*);

public:
	GameObject *owner = nullptr;
	Application *App = nullptr;
};
extern "C" SCRIPT_API Script* ScriptDLL();
typedef Script* (*CREATE_SCRIPT) ();
#endif