#ifndef _COMPONENTSCRIPT_H_
#define _COMPONENTSCRIPT_H_
#include "Component.h"
#include "UI/Panel/InspectorSubpanel/PanelComponent.h"
#include <unordered_map>
#include <string>

class Application;
class Script;
class ComponentScript :public Component
{
public:
	ComponentScript();
	ComponentScript(GameObject * owner, std::string script_name);
	//ComponentScript(GameObject*);
	~ComponentScript();
	void LoadName(std::string script_name);
	void Update();
	void Delete();

	void Save(Config& config) const{}
	void Load(const Config& config) {}
	void ShowComponentWindow();
	/*virtual void Start() {};
	virtual void Awake() {};
	virtual void Update() {};
	virtual void OnInspector() {};
	void SetAppPointer(Application*);*/
	//TODO: Implement more global functions Unity-like
public:
	std::string name;
	Script* script = nullptr;

private:
	friend class PanelComponent;
};
#endif //_COMPONENTSCRIPT_H_

