#ifndef _COMPONENTSCRIPT_H_
#define _COMPONENTSCRIPT_H_

#include "Component.h"
#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

#include <string>
#include <unordered_map>


class Application;
class Script;
class ComponentScript :public Component
{
public:
	ComponentScript();
	ComponentScript(GameObject* owner, std::string& script_name);
	~ComponentScript() = default;
	void LoadName(std::string& script_name);
	void Update();
	void Delete();
	void ShowComponentWindow();
	void Save(Config& config) const override;
	void Load(const Config& config) override;

public:
	std::string name;
	Script* script = nullptr;

private:
	friend class PanelComponent;
};
#endif //_COMPONENTSCRIPT_H_

