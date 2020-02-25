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
		float constant = 1.0F;
		float linear = 0.027F;
		float quadratic = 0.0028F;
	};

	struct SpotLight
	{
		float outer_cutoff = cos((12.5f * 3, 1415) / 180);
		float cutoff = cos((17.5f * 3, 1415) / 180);

		float constant = 1.0F;
		float linear = 0.027F;
		float quadratic = 0.0028F;
	};

	ComponentLight();
	ComponentLight(GameObject * owner);
	~ComponentLight() = default;

	void Delete() override;
	void Save(Config& config) const override;
	void Load(const Config &config) override;

	int GetLightTypeInt() const; //TODO ->refactor this.

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

