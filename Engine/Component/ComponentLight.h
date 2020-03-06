#ifndef _COMPONENTLIGHT_H
#define _COMPONENTLIGHT_H

#include "Component.h"
#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

class GameObject;
class ComponentLight : public Component
{
public:
	ComponentLight();
	ComponentLight(GameObject * owner);
	~ComponentLight() = default;
	
	//Copy and move
	ComponentLight(const ComponentLight& component_to_copy);
	ComponentLight(ComponentLight&& component_to_move);

	ComponentLight & operator=(const ComponentLight & component_to_copy);
	ComponentLight & operator=(ComponentLight && component_to_move);
	Component* Clone(bool create_on_module = true) const override;
	void Copy(Component * component_to_copy) const override { 
		*component_to_copy = *this;
		*static_cast<ComponentLight*>(component_to_copy) = *this; };
	void Delete() override;
	void Render() const;

	void Save(Config& config) const override;
	void Load(const Config &config) override;

public:
	float light_color[3] = { 1.0f, 1.0f, 1.0f};
	float light_intensity = 1.f; 

private:
	friend class PanelComponent;
};

#endif // !_COMPONENTLIGHT_H

