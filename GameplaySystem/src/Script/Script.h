#ifndef  __SCRIPT_H__
#define  __SCRIPT_H__
#ifdef SCRIPT_EXPORTS
#define SCRIPT_API __declspec(dllexport)
#else
#define SCRIPT_API __declspec(dllimport)
#endif

#define GET_VARIABLE_NAME(Variable) (#Variable)

#include "Helper/Config.h"

#include <string>


class GameObject;
class Application;
struct ImGuiContext;

class ComponentScript;
class PanelComponent;

class Script 
{
public:
	Script();
	virtual ~Script();

	virtual void Awake() {};
	virtual void Start() {};
	virtual void Update() {};

	virtual void OnInspector(ImGuiContext*) {};

	virtual void AddReferences(GameObject* owner, Application*);

	virtual void Save(Config& config) const;
	virtual void Load(const Config &config);
	virtual void Link();
	
	void ShowVariables();
	virtual void InitPublicGameObjects() {};

public:
	GameObject *owner = nullptr;
	Application *App = nullptr;
	PanelComponent *panel = nullptr;

protected:
	//Important
	std::vector<GameObject**> public_gameobjects;
	std::vector<uint64_t> go_uuids;
	std::vector<std::string> name_gameobjects;
	std::vector<std::string> variable_names;
	std::string is_object = "None";
};
extern "C" SCRIPT_API Script* ScriptDLL();
typedef Script* (*CREATE_SCRIPT) ();
#endif