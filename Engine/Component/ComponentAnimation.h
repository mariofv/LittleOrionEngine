#ifndef _COMPONENTANIMATION_H_
#define _COMPONENTANIMATION_H_

#include "Component.h"

#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

class AnimController;
class GameObject;

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

	void UpdateBone(GameObject* bone);

public:
	AnimController* animation_controller = nullptr;
	std::string animation_path = "";

private:
	friend class PanelComponent;
};

#endif //_COMPONENTANIMATION_H_

