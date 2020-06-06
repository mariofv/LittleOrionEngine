#ifndef _COMPONENTSCRIPT_H_
#define _COMPONENTSCRIPT_H_

#include "Component.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include <string>
#include <unordered_map>


class Application;
class Script;
class ComponentScript : public Component
{
public:
	ComponentScript();
	ComponentScript(GameObject* owner, std::string& script_name);
	~ComponentScript() = default;

	//Copy and move
	ComponentScript(const ComponentScript& component_to_copy) = default;
	ComponentScript(ComponentScript&& component_to_move) = default;

	ComponentScript & operator=(const ComponentScript & component_to_copy) = default;
	ComponentScript & operator=(ComponentScript && component_to_move) = default;

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	void Enable() override;

	void LoadName(std::string& script_name);

	void Update();
	void AwakeScript();
	void StartScript();
	void Delete();

	void ShowComponentWindow();

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

public:
	std::string name;
	Script* script = nullptr;
	bool awaken = false;
	bool started = false;

private:
	friend class PanelComponent;
};
#endif //_COMPONENTSCRIPT_H_

