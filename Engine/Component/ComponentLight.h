#ifndef _COMPONENTLIGHT_H
#define _COMPONENTLIGHT_H

#include "Component.h"
#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

class GameObject;
class ComponentLight : public Component
{
public:
	enum class LightType
	{
		POINT_LIGHT,
		SPOT_LIGHT,
		DIRECTIONAL_LIGHT
	};

	struct PointLight
	{
		float constant;
		float linear;
		float quadratic;
	};

	struct SpotLight
	{
		float outer_cutoff;
		float cutoff;

		float constant;
		float linear;
		float quadratic;
	};

	ComponentLight();
	ComponentLight(GameObject * owner);
	~ComponentLight() = default;

	void Delete() override;

	void Save(Config& config) const override;
	void Load(const Config &config) override;

public:
	float light_color[3] = { 1.0f, 1.0f, 1.0f};
	float light_intensity = 1.f; 
	LightType light_type = LightType::DIRECTIONAL_LIGHT;

	PointLight point_light_parameters;
	SpotLight spot_light_parameters;

private:
	friend class PanelComponent;
};

#endif // !_COMPONENTLIGHT_H

