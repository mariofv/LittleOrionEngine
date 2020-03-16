#ifndef _COMPONENTANIMATION_H_
#define _COMPONENTANIMATION_H_

#include "Component.h"

#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

class AnimControler;

class ComponentAnimation :	public Component
{
public:
	ComponentAnimation();
	ComponentAnimation(GameObject * owner);
	~ComponentAnimation();

	void Update() override;
	void Delete() override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void OnPlay();
	void OnStop();
	void OnUpdate();

public:
	AnimControler* animation_controller = nullptr;
	std::string animation_path = "";

private:
	friend class PanelComponent;
};

#endif //_COMPONENTANIMATION_H_

